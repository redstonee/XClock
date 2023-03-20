#include <Arduino.h>
#include "ClockKey.h"
#include "Sound/Sound.h"

OneButton *ClockKey::ButtonLeft = new OneButton(CLOCKKEY_LEFT_PIN);
OneButton *ClockKey::ButtonRight = new OneButton(CLOCKKEY_RIGHT_PIN);
OneButton *ClockKey::ButtonOk = new OneButton(CLOCKKEY_OK_PIN);
QueueHandle_t ClockKey::pKeySendQueue = NULL;
// ----- button 1 callback functions

// This function will be called when the button1 was pressed 1 time (and no 2. button press followed).
void ClockKey::vClickLeft() {
  boReqSound(enSndID_Alarm1,1);
  Serial.println("Button Left click.");
} // clickLeft


// This function will be called when the button1 was pressed 2 times in a short timeframe.
void ClockKey::vDoubleClickLeft() {
  boReqSound(enSndID_Alarm2,1);
  Serial.println("Button Left doubleclick.");
} // doubleclickLeft


// This function will be called once, when the button1 is pressed for a long time.
void ClockKey::vLongPressStartLeft() {
  boReqSound(enSndID_Ding,1);
  Serial.println("Button Left longPress start");
} // longPressStartLeft


// This function will be called often, while the button1 is pressed for a long time.
void ClockKey::vLongPressLeft() {
  Serial.println("Button Left longPress...");
} // longPressLeft


// This function will be called once, when the button1 is released after beeing pressed for a long time.
void ClockKey::vLongPressStopLeft() {
  Serial.println("Button Left longPress stop");
} // longPressStopLeft


// ... and the same for button 2:

void ClockKey::vClickRight() {
  boReqSound(enSndID_DingDong,1);
  Serial.println("Button Right click.");
} // clickRight


void ClockKey::vDoubleClickRight() {
  boReqSound(enSndID_Ding,1);
  Serial.println("Button Right doubleclick.");
} // doubleclickRight


void ClockKey::vLongPressStartRight() {
  boReqSound(enSndID_Ding,1);
  Serial.println("Button Right longPress start");
} // longPressStartRight


void ClockKey::vLongPressRight() {
  boReqSound(enSndID_Ding,1);
  Serial.println("Button Right longPress...");
} // longPressRight

void ClockKey::vLongPressStopRight() {
  Serial.println("Button Right longPress stop");
} // longPressStopRight

// ... and the same for button 3:

void ClockKey::vClickOk() {
    tstKeyEvent key={enKey_OK,enKey_ShortPress};
    SendKeyToQ(&key);
    boReqSound(enSndID_Ding,1);
    Serial.println("Button Ok click.");
} // clickOk


void ClockKey::vDoubleClickOk() {
  boReqSound(enSndID_Ding,1);
  Serial.println("Button Ok doubleclick.");
} // doubleclick3


void ClockKey::vLongPressStartOk() {
  boReqSound(enSndID_Ding,1);
  Serial.println("Button Ok longPress start");
} // longPressStartOk


void ClockKey::vLongPressOk() {
  boReqSound(enSndID_Ding,1);
  Serial.println("Button Ok longPress...");
} // longPressOk

void ClockKey::vLongPressStopOk() {
  Serial.println("Button Ok longPress stop");
} // longPressStopOk

// End

void ClockKey::KeyLoop(void *param){
    for(;;){
        vTaskDelay(10);
        ButtonLeft->tick();
        ButtonRight->tick();
        ButtonOk->tick(); 
    }    
}

void ClockKey::Start(){
    xTaskCreate(
    KeyLoop,    // Function that should be called
    "Keyhandler main task",   // Name of the task (for debugging)
    2000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );
}

void ClockKey::SetSendQueue(QueueHandle_t &Queue)
{
    pKeySendQueue = Queue;
}

bool ClockKey::SendKeyToQ(tstKeyEvent *key)
{
    if( pKeySendQueue != NULL )
    {
        if( xQueueSend( pKeySendQueue,
                       ( void * ) key,
                       ( TickType_t ) 10 ) != pdPASS )
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
    ButtonLeft->setClickTicks(80);
    ButtonLeft->attachDoubleClick(vDoubleClickLeft);
    ButtonLeft->attachLongPressStart(vLongPressStartLeft);
    ButtonLeft->attachLongPressStop(vLongPressStopLeft);
    ButtonLeft->attachDuringLongPress(vLongPressLeft);

    // link the button 2 functions.
    ButtonRight->attachClick(vClickRight);
    ButtonRight->setClickTicks(80);
    ButtonRight->attachDoubleClick(vDoubleClickRight);
    ButtonRight->attachLongPressStart(vLongPressStartRight);
    ButtonRight->attachLongPressStop(vLongPressStopRight);
    ButtonRight->attachDuringLongPress(vLongPressRight);

    // link the button 3 functions.
    ButtonOk->attachClick(vClickOk);
    ButtonOk->setClickTicks(80);
    ButtonOk->attachDoubleClick(vDoubleClickOk);
    ButtonOk->attachLongPressStart(vLongPressStartOk);
    ButtonOk->attachLongPressStop(vLongPressStopOk);
    ButtonOk->attachDuringLongPress(vLongPressOk);
}

ClockKey::~ClockKey()
{
    if(ButtonLeft)
    {
        free(ButtonLeft);
    }
    if(ButtonRight)
    {
        free(ButtonRight);
    }
    if(ButtonOk)
    {
        free(ButtonOk);
    }
}
