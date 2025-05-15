#include <WiFi.h>
#include <Arduino.h>
#include <DNSServer.h>
#include <WebServer.h>

#include "Network.h"

extern const char *webPageStart;
extern const char *webPageEnd;
extern const char *noSSIDPage;
extern const char *noPSKPage;

namespace Network
{
  static const char *TAG = "web";
  static const char *ntpServer1 = "ntp1.aliyun.com";
  static const char *ntpServer2 = "ntp2.aliyun.com";
  static const char *ntpServer3 = "ntp3.aliyun.com";

  static const char *AP_SSID = "XClock";
  static const IPAddress apIP(192, 168, 4, 1);
  constexpr auto GMT_OFFSET = 8 * 3600;
  constexpr auto DAYLIGHT_OFFSET = 0;
  constexpr auto WIFI_CONFIG_TIMEOUT = 180 * 1000; // in milliseconds

  static DNSServer dnsServer;
  static WebServer webServer(80);
  static WiFiStatus wifiStatus = WiFiStatus::DISCONNECTED;

  static String scanResultHtml;

  void handleRoot()
  {
    webServer.send(200, "text/html", webPageStart + scanResultHtml + webPageEnd);
  }

  void handleConfigWifi()
  {
    String ssid, psk;
    if (webServer.hasArg("ssid"))
    {
      ssid = webServer.arg("ssid");
      ESP_LOGD(TAG, "SSID: %s", ssid.c_str());
    }
    else
    {
      ESP_LOGE(TAG, "SSID not found");
      webServer.send(200, "text/html", noSSIDPage);
      return;
    }
    if (webServer.hasArg("pass"))
    {
      psk = webServer.arg("pass");
    }
    else
    {
      ESP_LOGE(TAG, "Password not found");
      webServer.send(200, "text/html", noPSKPage);
      return;
    }

    webServer.send(200, "text/html", "Connecting to " + ssid + ", you can close this page.");
    delay(2000);
    WiFi.softAPdisconnect(true);
    webServer.close();

    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, psk);
  }

  void wifiConfigTask(void *pvParameters)
  {
    wifiStatus = WiFiStatus::CONFIGURING;
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, IPAddress(), IPAddress(255, 255, 255, 0));
    if (WiFi.softAP(AP_SSID))
    {
      ESP_LOGI(TAG, "AP mode started with SSID: %s.", AP_SSID);
    }
    else
    {
      ESP_LOGE(TAG, "Failed to start AP mode.");
      vTaskDelete(nullptr);
      return;
    }
    dnsServer.start(53, "*", apIP);

    webServer.on("/", HTTP_GET, handleRoot);
    webServer.on("/configwifi", HTTP_POST, handleConfigWifi);
    webServer.begin();
    ESP_LOGI(TAG, "WebServer started!");

    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i)
    {
      scanResultHtml += "<li>" + String(WiFi.SSID(i)) + " (" + String(WiFi.RSSI(i)) + " dBm)</li>";
    }

    auto startTime = millis();
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (millis() - startTime < WIFI_CONFIG_TIMEOUT)
    {
      dnsServer.processNextRequest();
      webServer.handleClient();

      // Check if WiFi is configured and break the loop if it is
      if (WiFi.getMode() == WIFI_MODE_STA)
      {
        wifiStatus = WiFi.status() == WL_CONNECTED ? WiFiStatus::CONNECTED : WiFiStatus::DISCONNECTED;
        break;
      }

      xTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
    }

    if (WiFi.status() != WL_CONNECTED)
    {
      ESP_LOGW(TAG, "WiFi Config failed.");
      wifiStatus = WiFiStatus::DISCONNECTED;
    }
    dnsServer.stop();

    vTaskDelete(nullptr);
  }

  void checkWiFiStatus(TimerHandle_t)
  {
    if (WiFi.status() == WL_CONNECTED)
      wifiStatus = WiFiStatus::CONNECTED;
    else if (wifiStatus != WiFiStatus::CONFIGURING)
      wifiStatus = WiFiStatus::DISCONNECTED;
  }

  void begin()
  {

    WiFi.begin();
    configTime(GMT_OFFSET, DAYLIGHT_OFFSET, ntpServer1, ntpServer2, ntpServer3);

    auto checkWiFiTimer = xTimerCreate("Check WiFi Status Timer", pdMS_TO_TICKS(500),
                                       true, nullptr, checkWiFiStatus);
    xTimerStart(checkWiFiTimer, 0);
  }

  WiFiStatus getWiFiStatus()
  {
    return wifiStatus;
  }

  void runAPConfig()
  {
    if (wifiStatus == WiFiStatus::CONFIGURING)
      return; // Already in configuring state
    xTaskCreate(wifiConfigTask, "WiFi config Task", 3072, nullptr, 1, nullptr);
  }

  String getSSID()
  {
    return WiFi.SSID();
  }
} // namespace Network
