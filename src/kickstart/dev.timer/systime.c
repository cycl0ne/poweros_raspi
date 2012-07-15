#include "timer_intern.h"

#include "raspi_platform.h"
#include "exec_funcs.h"

void timer_GetSysTime(struct TimerBase *TimerBase, struct TimeVal *src)
{
	Disable();
	src->tv_micro = TimerBase->CurrentTime.tv_micro;
	src->tv_secs  = TimerBase->CurrentTime.tv_secs;
	Enable();
}
#define STC_FREQ_HZ  1000000

UINT32 timer_ReadEClock(struct TimerBase *TimerBase, struct EClockVal *src)
{
	src->ev_lo = READ32(ST_BASE+0x04);
	src->ev_hi = READ32(ST_BASE+0x08);
	
	return STC_FREQ_HZ;
}

