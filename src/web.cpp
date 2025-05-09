#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <ESPmDNS.h>  //用于设备域名 MDNS.begin("esp32")
#include <esp_wifi.h> //用于esp_wifi_restore() 删除保存的wifi信息
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "web.h"
#include "main.h"
#include "SD3078.hpp"

constexpr auto DNS_PORT = 53;      // 设置DNS端口号
const char *AP_SSID = "XClock_AP"; // 设置AP热点名称
const char *HOST_NAME = "XClock";  // 设置设备名
String scanNetworksID = "";        // 用于储存扫描到的WiFi ID
int connectTimeOut = 15000;        // Timeout for WiFi connection in milliseconds
IPAddress apIP(192, 168, 4, 1);    // 设置AP的IP地址
String wifi_ssid = "";             // 暂时存储wifi账号密码
String wifi_pass = "";             // 暂时存储wifi账号密码
bool WifiConfiguring = false;
const char *ntpServer1 = "ntp1.aliyun.com";
const char *ntpServer2 = "ntp2.aliyun.com";
const char *ntpServer3 = "ntp3.aliyun.com";
const long gmtOffset_sec = 8 * 3600;
const int daylightOffset_sec = 0;
TimerHandle_t wifiCconnectTimer = nullptr;
/*天气*/
const char *Weatherhost = "api.seniverse.com"; // 服务器地址
const int WeatherhttpPort = 80;                // 端口号
String reqUserKey = "S7KrrqZRFf8fC52mJ";       // 知心天气API私钥
String reqLocation = "南京";                   // 地址
String reqUnit = "c";                          // 摄氏度
                                               //-------------------http请求-----------------------------//
String reqRes = "/v3/weather/now.json?key=" + reqUserKey +
                +"&location=" + reqLocation +
                "&language=en&unit=" + reqUnit;
String httprequest = String("GET ") + reqRes + " HTTP/1.1\r\n" +
                     "Host: " + Weatherhost + "\r\n" +
                     "Connection: close\r\n\r\n";

// 定义根目录首页网页HTML源代码
#define ROOT_HTML "<!DOCTYPE html><html><head><title>XClock WIFI Config</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.input{display: block; margin-top: 10px;}.input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}.input input{height: 30px;width: 200px;}.btn{width: 120px; height: 35px; background-color: #000000; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}</style><body><form method=\"POST\" action=\"configwifi\"><label class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\" value=\"\"></label><label class=\"input\"><span>WiFi PASS</span><input type=\"text\"  name=\"pass\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"Submie\"> <p><span> Nearby wifi:</P></form>"
// 定义成功页面HTML源代码
#define SUCCESS_HTML "<html><body><font size=\"10\">successd,wifi connecting...<br />Please close this page manually.</font></body></html>"

static const char *TAG = "web";

DNSServer dnsServer; // 创建dnsServer实例
WebServer server;

void WeatherRequest();
void parseWeatherJson(WiFiClient client);

// 初始化AP模式
void initSoftAP()
{
  WiFi.mode(WIFI_AP);                                         // 配置为AP模式
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); // 设置AP热点IP和子网掩码
  if (WiFi.softAP(AP_SSID))
  { // 开启AP热点,如需要密码则添加第二个参数
    // 打印相关信息
    ESP_LOGI(TAG, "ESP-32S SoftAP is right.");
    ESP_LOGI(TAG, "Soft-AP IP address = %s", WiFi.softAPIP().toString().c_str());
  }
  else
  { // 开启热点失败
    ESP_LOGE(TAG, "WiFiAP Failed");
  }
}

// 初始化DNS服务器
void initDNS()
{
  // 判断将所有地址映射到esp32的ip上是否成功
  if (dnsServer.start(DNS_PORT, "*", apIP))
  {
    ESP_LOGI(TAG, "start dnsserver success.");
  }
  else
  {
    ESP_LOGE(TAG, "start dnsserver failed.");
  }
}

// 扫描WiFi
bool scanWiFi()
{
  ESP_LOGI(TAG, "scan start");
  // 扫描附近WiFi
  int n = WiFi.scanNetworks();
  ESP_LOGI(TAG, "scan done");
  if (n == 0)
  {
    ESP_LOGW(TAG, "no networks found");
    scanNetworksID = "no networks found";
    return false;
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      scanNetworksID += "<P>" + WiFi.SSID(i) + "</P>";
      delay(10);
    }
    return true;
  }
}

// 处理网站根目录“/”(首页)的访问请求,将显示配置wifi的HTML页面
void handleRoot()
{
  if (server.hasArg("selectSSID"))
  {
    server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
  }
  else
  {
    server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
  }
}

void handleNotFound()
{               // 当浏览器请求的网络资源无法在服务器找到时通过此自定义函数处理
  handleRoot(); // 访问不存在目录则返回配置页面
  //   server.send(404, "text/plain", "404: Not found");
}

void clearWiFiConfig()
{
  esp_wifi_restore(); // 删除保存的wifi信息
  Preferences pref;
  pref.begin(PrefKey_WifiConfiged);
  pref.putBool(PrefKey_WifiConfiged, false);
  pref.end();
  ESP_LOGI(TAG, "WiFi config cleared.");
}

bool isWifiConfigured(void)
{
  Preferences pref;
  bool res = false;
  pref.begin(PrefKey_WifiConfiged);
  res = pref.getBool(PrefKey_WifiConfiged, false);
  pref.end();
  return res;
}

String getSSIDConfig(void)
{
  String buf;
  Preferences pref;
  pref.begin(PrefKey_WifiSSID);
  buf = pref.getString(PrefKey_WifiSSID, "None");
  pref.end();
  Serial.println(buf);
  return buf;
}

void SetCurWeatherCode(uint8_t code)
{
  Preferences pref;
  pref.begin(PrefKey_WeatherSpace);
  pref.putUChar(PrefKey_WeatherCodeCur, code);
  pref.end();
}

uint8_t GetCurWeatherCode()
{
  int code = 0;
  Preferences pref;
  pref.begin(PrefKey_WeatherSpace);
  code = pref.getUChar(PrefKey_WeatherCodeCur, 0);
  pref.end();
  return code;
}

void WeatherRequest()
{
  WiFiClient client;
  // 1 连接服务器
  if (client.connect(Weatherhost, WeatherhttpPort))
  {
    ESP_LOGD(TAG, "Success connect to weather server");
    client.print(httprequest); // 访问API接口
    String response_status = client.readStringUntil('\n');
    ESP_LOGD(TAG, "Weather response status: %s", response_status.c_str());

    if (client.find("\r\n\r\n"))
    {
      ESP_LOGD(TAG, "Try to parse weather json data");
      parseWeatherJson(client);
      client.stop();
    }
  }
  else
  {
    ESP_LOGE(TAG, "Connect to weather server failed");
  }
}

void parseWeatherJson(WiFiClient client)
{
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 230;
  DynamicJsonDocument doc(capacity);
  deserializeJson(doc, client);

  JsonObject obj1 = doc["results"][0];
  String cityName = obj1["location"]["name"].as<String>();
  String weather = obj1["now"]["text"].as<String>();
  String code = obj1["now"]["code"].as<String>();
  String temperature = obj1["now"]["temperature"].as<String>();
  int code_int = obj1["now"]["code"].as<int>();
  ESP_LOGD(TAG, "City: %s", cityName.c_str());
  ESP_LOGD(TAG, "Weather: %s", weather.c_str());
  Serial.println(cityName);
  Serial.println(code);
  // Serial.println(weather);
  Serial.println(temperature);
  SetCurWeatherCode(code_int);
  if (pdPASS != xTimerStop(wifiCconnectTimer, 10))
  {
    Serial.println("Stop timer failed");
  }
  Serial.println("Get weather,sleep");
  ClearWakeupRequest(false);
}

// void vConnectTOCb(TimerHandle_t xTimer)
// {
//   ESP_LOGI(TAG, "WiFi connection timeout.");
//   WiFi.disconnect();
//   WiFi.mode(WIFI_OFF);
//   ClearWakeupRequest(false);
// }

/**
 * @brief Try to connect to WiFi, if failed, start AP mode for web config.
 *
 * @param timeout Connection timeout in milliseconds.
 */
void connectToWiFi(int timeout)
{
  // 设置为STA模式并连接WIFI
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true); // 设置自动连接
  // 用字符串成员函数c_str()生成一个const char*指针，指向以空字符终止的数组,即获取该字符串的指针。
  if (wifi_ssid != "")
  {
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
    wifi_ssid = "";
    wifi_pass = "";
  }
  else
  {
    WiFi.begin(); // 连接上一次连接成功的wifi
  }
  auto startTime = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);

    // Cannot connect to WiFi for too long
    if (millis() - startTime > timeout)
    {
      ESP_LOGW(TAG, "Failed to connect to WiFi, starting AP for web config.");
      WifiConfiguring = true;
      wifiConfig();
      return;
    }
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    WifiConfiguring = false;
    ESP_LOGI(TAG, "Connected to WiFi: %s", WiFi.SSID().c_str());
    server.stop();
    Preferences pref;
    pref.begin(PrefKey_WifiConfiged);
    pref.putBool(PrefKey_WifiConfiged, true);
    Preferences pref_ssid;
    pref_ssid.begin(PrefKey_WifiSSID);
    pref_ssid.putString(PrefKey_WifiSSID, WiFi.SSID().c_str());
    pref_ssid.end();
  }
}

void handleConfigWifi()
{
  // 返回http状态
  // server.send(200, "text/html", SUCCESS_HTML);
  if (server.hasArg("ssid"))
  { // 判断是否有账号参数
    Serial.print("got ssid:");
    wifi_ssid = server.arg("ssid"); // 获取html表单输入框name名为"ssid"的内容
    // strcpy(sta_ssid, server.arg("ssid").c_str());//将账号参数拷贝到sta_ssid中
    Serial.println(wifi_ssid);
  }
  else
  { // 没有参数
    Serial.println("error, not found ssid");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid"); // 返回错误页面
    return;
  }
  if (server.hasArg("pass"))
  {
    Serial.print("got password:");
    wifi_pass = server.arg("pass"); // 获取html表单输入框name名为"pwd"的内容
    // strcpy(sta_pass, server.arg("pass").c_str());
    Serial.println(wifi_pass);
  }
  else
  {
    Serial.println("error, not found password");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }
  server.send(200, "text/html", "<meta charset='UTF-8'>SSID:" + wifi_ssid + "<br />password:" + wifi_pass + "<br />已取得WiFi信息,正在尝试连接,请手动关闭此页面。"); // 返回保存成功页面
  delay(2000);
  WiFi.softAPdisconnect(true); // 参数设置为true，设备将直接关闭接入点模式，即关闭设备所建立的WiFi网络。
  server.close();              // 关闭web服务
  WiFi.softAPdisconnect();     // 在不输入参数的情况下调用该函数,将关闭接入点模式,并将当前配置的AP热点网络名和密码设置为空值.
  Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass);
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("开始调用连接函数connectToWiFi()..");
    connectToWiFi(connectTimeOut);
  }
  else
  {
    Serial.println("提交的配置信息自动连接成功..");
    WifiConfiguring = false;
    ClearWakeupRequest(false);
  }
}

void initWebServer()
{
  if (MDNS.begin("esp32"))
  {
    ESP_LOGI(TAG, "MDNS responder started");
  }
  // 必须添加第二个参数HTTP_GET，以下面这种格式去写，否则无法强制门户
  server.on("/", HTTP_GET, handleRoot);                  //  当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
  server.on("/configwifi", HTTP_POST, handleConfigWifi); //  当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理
  server.onNotFound(handleNotFound);                     // 当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理
  // Tells the server to begin listening for incoming connections.Returns None
  server.begin(); // 启动TCP SERVER
  // server.setNoDelay(true);                                  //关闭延时发送
  ESP_LOGI(TAG, "WebServer started!");
}

// 用于配置WiFi
void wifiConfig()
{
  initSoftAP();
  initDNS();
  initWebServer();
  scanWiFi();
}

// void SetupWifi(void)
// {
//   if (WifiConfiguring != true)
//   {
//     RequestWakeup(false);
//     wifiCconnectTimer = xTimerCreate("ConnectWifiTimer", pdMS_TO_TICKS(1000 * 20),
//                                      pdFALSE, nullptr, vConnectTOCb);
//     xTimerStart(wifiCconnectTimer, 10);
//     WiFi.hostname(HOST_NAME);
//     connectToWiFi(connectTimeOut);
//     configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
//     WeatherRequest();
//     WiFi.disconnect();
//     WiFi.mode(WIFI_OFF);
//   }
// }


void handleNetworkEvents(TimerHandle_t)
{
  dnsServer.processNextRequest(); // 检查客户端DNS请求
  server.handleClient();          // 检查客户端(浏览器)http请求
}

void initWiFi(void)
{
  WiFi.hostname(HOST_NAME); // 设置设备名
  connectToWiFi(connectTimeOut);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);

  auto networkEventTimer = xTimerCreate("Network fucking timer", pdMS_TO_TICKS(1000),
                                        true, nullptr, handleNetworkEvents);
  xTimerStart(networkEventTimer, 0);
}
