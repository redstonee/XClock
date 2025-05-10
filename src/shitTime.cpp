#include "shitTime.h"
#include <sys/time.h>

void setSystemTime(tm &time)
{
    auto t = mktime(&time);
    struct timeval now = {.tv_sec = t, .tv_usec = 0};
    settimeofday(&now, NULL);
}