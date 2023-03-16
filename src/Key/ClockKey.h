#ifndef __ClockKey_H__
#define __ClockKey_H__

#include "OneButton.h"

#define CLOCKKEY_LEFT_PIN       32
#define CLOCKKEY_RIGHT_PIN      25
#define CLOCKKEY_OK_PIN         33

typedef enum
{
    enKey_Left,
    enKey_Right,
    enKey_OK,
}tenKeyID;

typedef enum
{
    enKey_ShortPress,
    enKey_LongPress,
    enKey_DoubleClick,
    enKey_LongPressStop,
}tenKeyEventType;

typedef struct
{
    tenKeyID Key;
    tenKeyEventType Type;
}tstKeyEvent;

class ClockKey
{
private:
    QueueHandle_t pKeySendQueue = NULL;
    static void vClickLeft(void);
    static void vDoubleClickLeft(void);
    static void vLongPressStartLeft(void);
    static void vLongPressLeft(void);
    static void vLongPressStopLeft(void);

    static void vClickRight(void);
    static void vDoubleClickRight(void);
    static void vLongPressStartRight(void);
    static void vLongPressRight(void);
    static void vLongPressStopRight(void);

    static void vClickOk(void);
    static void vDoubleClickOk(void);
    static void vLongPressStartOk(void);
    static void vLongPressOk(void);
    static void vLongPressStopOk(void);  
    static void KeyLoop(void *param);  
public:
    static OneButton *ButtonLeft;
    static OneButton *ButtonRight;
    static OneButton *ButtonOk;
    void SetSendQueue(QueueHandle_t &Queue);
    ClockKey();
    ~ClockKey();    
    void Start(void);
};


#endif