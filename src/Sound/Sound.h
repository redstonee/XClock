#ifndef __SOUND_H__
#define __SOUND_H__

typedef enum
{
    enSndID_None,
    enSndID_DingDong,
    enSndID_Alarm1,
    enSndID_Alarm2,
    enSndID_Ding,
    enSndID_Boo,
    enSndID_TotalNum,
}tenSoundID;

bool boReqSound(tenSoundID SndID, uint16_t times);

tenSoundID enGetCurSndID(void);

void vStopSound(void);

void vSoundInit(void);

#endif //__SOUND_H__