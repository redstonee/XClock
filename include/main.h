#ifndef __MAIN__
#define __MAIN__

#define AlarmRecallTimeout (portTICK_PERIOD_MS * 1000 * 60 * 2)

void RequestWakeup(bool boHMIDis);
void ClearWakeupRequest(bool boHMIDis);

#endif