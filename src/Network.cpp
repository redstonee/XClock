#include <WiFi.h>
#include <Arduino.h>
#include "Network.h"

namespace Network
{
  static const char *TAG = "web";
  static const char *ntpServer1 = "ntp1.aliyun.com";
  static const char *ntpServer2 = "ntp2.aliyun.com";
  static const char *ntpServer3 = "ntp3.aliyun.com";

  constexpr auto GMT_OFFSET = 8 * 3600;
  constexpr auto DAYLIGHT_OFFSET = 0;
  constexpr auto SMART_CONFIG_TIMEOUT_MS = 60000; // in milliseconds

  static WiFiStatus wifiStatus = WiFiStatus::DISCONNECTED;

  void smartConfigTask(void *pvParameters)
  {
    wifiStatus = WiFiStatus::CONFIGURING;
    WiFi.mode(WIFI_AP_STA);
    if (!WiFi.beginSmartConfig(SC_TYPE_ESPTOUCH_V2))
    {
      ESP_LOGE(TAG, "Failed to start SmartConfig.");
      vTaskDelete(nullptr);
      return;
    }

    auto startTime = millis();
    while (millis() - startTime < SMART_CONFIG_TIMEOUT_MS)
    {
      ESP_LOGI(TAG, "Waiting for SmartConfig.");
      if (WiFi.smartConfigDone())
      {
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
        Serial.println("SmartConfig received.");
        break;
      }
      delay(1000);
    }

    WiFi.stopSmartConfig();
    if (WiFi.status() != WL_CONNECTED)
    {
      ESP_LOGW(TAG, "SmartConfig failed.");
      WiFi.disconnect();
      wifiStatus = WiFiStatus::DISCONNECTED;
    }

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

  void runSmartConfig()
  {
    if (wifiStatus == WiFiStatus::CONFIGURING)
      return; // Already in configuring state

    xTaskCreate(smartConfigTask, "SmartConfig Task", 2048, nullptr, 1, nullptr);
  }

  String getSSID()
  {
    return WiFi.SSID();
  }
} // namespace Network
