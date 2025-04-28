#include <Arduino.h>
#include "ClockKey.h"
#include "Sound.h"
#include "config.h"

OneButton *ClockKey::ButtonLeft = new OneButton(BUTTON_LEFT_PIN);
OneButton *ClockKey::ButtonRight = new OneButton(BUTTON_RIGHT_PIN);
OneButton *ClockKey::ButtonOk = new OneButton(BUTTON_OK_PIN);
QueueHandle_t ClockKey::pKeySendQueue = NULL;

static const char *TAG = "ClockKey";
// ----- button 1 callback functions

// This function will be called when the button1 was pressed 1 time (and no 2. button press followed).
void ClockKey::vClickLeft()
{
  // boReqSound(enSndID_Alarm1,1);
  tstKeyEvent key = {enKey_Left, enKey_ShortPress};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Left click.");
} // clickLeft

// This function will be called when the button1 was pressed 2 times in a short timeframe.
void ClockKey::vDoubleClickLeft()
{
  // boReqSound(enSndID_Alarm2,1);
  tstKeyEvent key = {enKey_Left, enKey_DoubleClick};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Left doubleclick.");
} // doubleclickLeft

// This function will be called once, when the button1 is pressed for a long time.
void ClockKey::vLongPressStartLeft()
{
  // boReqSound(enSndID_Ding,1);
  tstKeyEvent key = {enKey_Left, enKey_LongPressStart};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Left longPress start");
} // longPressStartLeft

// This function will be called often, while the button1 is pressed for a long time.
void ClockKey::vLongPressLeft()
{
  static uint16_t u16LongPresDelay = 0;
  if (++u16LongPresDelay >= DurLongPressDelay)
  {
    u16LongPresDelay = 0;
    tstKeyEvent key = {enKey_Left, enKey_LongPress};
    SendKeyToQ(&key);
    ESP_LOGI(TAG, "Button Left longPress...");
  }

} // longPressLeft

// This function will be called once, when the button1 is released after beeing pressed for a long time.
void ClockKey::vLongPressStopLeft()
{
  tstKeyEvent key = {enKey_Left, enKey_LongPressStop};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Left longPress stop");
} // longPressStopLeft

// ... and the same for button 2:

void ClockKey::vClickRight()
{
  // boReqSound(enSndID_DingDong,1);
  tstKeyEvent key = {enKey_Right, enKey_ShortPress};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Right click.");
} // clickRight

void ClockKey::vDoubleClickRight()
{
  // boReqSound(enSndID_Ding,1);
  tstKeyEvent key = {enKey_Right, enKey_DoubleClick};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Right doubleclick.");
} // doubleclickRight

void ClockKey::vLongPressStartRight()
{
  // boReqSound(enSndID_Ding,1);
  tstKeyEvent key = {enKey_Right, enKey_LongPressStart};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Right longPress start");
} // longPressStartRight

void ClockKey::vLongPressRight()
{
  // boReqSound(enSndID_Ding,1);
  static uint16_t u16LongPresDelay = 0;
  if (++u16LongPresDelay >= DurLongPressDelay)
  {
    u16LongPresDelay = 0;
    tstKeyEvent key = {enKey_Right, enKey_LongPress};
    SendKeyToQ(&key);
    ESP_LOGI(TAG, "Button Right longPress...");
  }
} // longPressRight

void ClockKey::vLongPressStopRight()
{
  tstKeyEvent key = {enKey_Right, enKey_LongPressStop};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Right longPress stop");
} // longPressStopRight

// ... and the same for button 3:

void ClockKey::vClickOk()
{
  tstKeyEvent key = {enKey_OK, enKey_ShortPress};
  SendKeyToQ(&key);
  // boReqSound(enSndID_Ding,1);
  ESP_LOGI(TAG, "Button Ok click.");
} // clickOk

void ClockKey::vDoubleClickOk()
{
  // boReqSound(enSndID_Ding,1);
  tstKeyEvent key = {enKey_OK, enKey_DoubleClick};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Ok doubleclick.");
} // doubleclick3

void ClockKey::vLongPressStartOk()
{
  // boReqSound(enSndID_Ding,1);
  tstKeyEvent key = {enKey_OK, enKey_LongPressStart};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Ok longPress start");
} // longPressStartOk

void ClockKey::vLongPressOk()
{
  // boReqSound(enSndID_Ding,1);
  static uint16_t u16LongPresDelay = 0;
  if (++u16LongPresDelay >= DurLongPressDelay)
  {
    tstKeyEvent key = {enKey_OK, enKey_LongPress};
    SendKeyToQ(&key);
    ESP_LOGI(TAG, "Button Ok longPress...");
  }
} // longPressOk

void ClockKey::vLongPressStopOk()
{
  tstKeyEvent key = {enKey_OK, enKey_LongPressStop};
  SendKeyToQ(&key);
  ESP_LOGI(TAG, "Button Ok longPress stop");
} // longPressStopOk

// End

void ClockKey::KeyLoop(void *param)
{
  for (;;)
  {
    vTaskDelay(10);
    ButtonLeft->tick();
    ButtonRight->tick();
    ButtonOk->tick();
  }
}

void ClockKey::Start()
{
  xTaskCreate(
      KeyLoop,                // Function that should be called
      "Keyhandler main task", // Name of the task (for debugging)
      1000,                   // Stack size (bytes)
      NULL,                   // Parameter to pass
      5,                      // Task priority
      NULL                    // Task handle
  );
}

void ClockKey::SetSendQueue(QueueHandle_t &Queue)
{
  pKeySendQueue = Queue;
}

bool ClockKey::SendKeyToQ(tstKeyEvent *key)
{
  if (pKeySendQueue != NULL)
  {
    if (xQueueSend(pKeySendQueue,
                   (void *)key,
                   (TickType_t)10) != pdPASS)
    {
      /* Failed to post the message, even after 10 ticks. */
      return false;
    }
    return true;
  }
  else
  {
    return false;
  }
}

ClockKey::ClockKey()
{
  // link the button 1 functions.
  ButtonLeft->attachClick(vClickLeft);
  ButtonLeft->setClickMs(150);
  ButtonLeft->attachDoubleClick(vDoubleClickLeft);
  ButtonLeft->attachLongPressStart(vLongPressStartLeft);
  ButtonLeft->attachLongPressStop(vLongPressStopLeft);
  ButtonLeft->attachDuringLongPress(vLongPressLeft);

  // link the button 2 functions.
  ButtonRight->attachClick(vClickRight);
  ButtonRight->setClickMs(150);
  ButtonRight->attachDoubleClick(vDoubleClickRight);
  ButtonRight->attachLongPressStart(vLongPressStartRight);
  ButtonRight->attachLongPressStop(vLongPressStopRight);
  ButtonRight->attachDuringLongPress(vLongPressRight);

  // link the button 3 functions.
  ButtonOk->attachClick(vClickOk);
  ButtonOk->setClickMs(150);
  ButtonOk->attachDoubleClick(vDoubleClickOk);
  ButtonOk->attachLongPressStart(vLongPressStartOk);
  ButtonOk->attachLongPressStop(vLongPressStopOk);
  ButtonOk->attachDuringLongPress(vLongPressOk);
}

ClockKey::~ClockKey()
{
  if (ButtonLeft)
  {
    free(ButtonLeft);
  }
  if (ButtonRight)
  {
    free(ButtonRight);
  }
  if (ButtonOk)
  {
    free(ButtonOk);
  }
}
