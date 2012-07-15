#include "timer_intern.h"

INT32 timer_CmpTime(struct TimerBase *TimerBase, struct TimeVal *src, struct TimeVal *dest)
{
    INT32 diff;

    if (dest->tv_secs == src->tv_secs) diff = src->tv_micro - dest->tv_micro;
    else diff = src->tv_secs - dest->tv_secs;

    if (diff < 0) return -1;
    else if (diff > 0) return 1;
    else return 0;
}

void timer_AddTime(struct TimerBase *TimerBase, struct TimeVal *src, struct TimeVal *dst)
{
	dst->tv_micro += src->tv_micro;
	dst->tv_secs  += src->tv_secs;
	while(dst->tv_micro > 999999)
	{
		dst->tv_secs++;
		dst->tv_micro -= 1000000;
	}
}

void timer_SubTime(struct TimerBase *TimerBase, struct TimeVal *src, struct TimeVal *dest)
{
    while(src->tv_micro > 999999)
    {
		src->tv_secs++;
		src->tv_micro -= 1000000;
    }
    while(dest->tv_micro > 999999)
    {
		dest->tv_secs++;
		dest->tv_micro -= 1000000;
    }
    if(dest->tv_micro < src->tv_micro)
    {
		dest->tv_micro += 1000000;
		dest->tv_secs--;
    }

    dest->tv_micro -= src->tv_micro;
    dest->tv_secs -= src->tv_secs;
}


