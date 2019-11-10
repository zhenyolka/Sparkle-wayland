#include "were_log.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <cstdint>


const int in_day = 1000 * 60 * 60 * 24;
const int in_hour = 1000 * 60 * 60;
const int in_minute = 1000 * 60;
const int in_second = 1000;


void were_log(const char *format, ...)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    uint64_t milliseconds__ = 0;
    milliseconds__ += 1000LL * ts.tv_sec;
    milliseconds__ += ts.tv_nsec / 1000000LL;

    milliseconds__ = milliseconds__ % in_day;
    int hours = milliseconds__ / in_hour;
    milliseconds__ = milliseconds__ % in_hour;
    int minutes = milliseconds__ / in_minute;
    milliseconds__ = milliseconds__ % in_minute;
    int seconds = milliseconds__ / in_second;
    milliseconds__ = milliseconds__ % in_second;
    int milliseconds = milliseconds__;

    fprintf(stdout, "%02d:%02d:%02d.%03d ", hours, minutes, seconds, milliseconds);

    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}
