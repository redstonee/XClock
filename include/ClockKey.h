#ifndef __ClockKey_H__
#define __ClockKey_H__

#include "OneButton.h"

#define DurLongPressDelay (10)

typedef enum
{
    enKey_Nokey,
    enKey_Left,
    enKey_Right,
    enKey_OK,
} tenKeyID;

typedef enum
{
    enKey_NoAct,
    enKey_ShortPress,
    enKey_DoubleClick,
    enKey_LongPressStart,
    enKey_LongPressStop,
    enKey_LongPress,
} tenKeyEventType;

typedef struct
{
    tenKeyID Key;
    tenKeyEventType Type;
} tstKeyEvent;

class ClockKey
{
private:
    static QueueHandle_t pKeySendQueue;
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
    static void tickKeys(TimerHandle_t);
    static bool SendKeyToQ(tstKeyEvent *);

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