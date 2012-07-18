#include "timer_intern.h"
#include "list.h"

#undef SysBase

#include "exec_funcs.h"

#define FastAddTime(d, s)\
    (d)->tv_micro += (s)->tv_micro;\
    (d)->tv_secs += (s)->tv_secs;\
    if((d)->tv_micro > 999999) {\
	(d)->tv_secs++;\
	(d)->tv_micro -= 1000000;\
    }

void lib_hexstrings ( unsigned int d );

	
__attribute__((no_instrument_function)) BOOL Timer3IRQServer(UINT32 number, istate* istate, TimerBase *TimerBase, APTR SysBase)
{
    FastAddTime(&TimerBase->CurrentTime, &TimerBase->VBlankTime);
    FastAddTime(&TimerBase->Elapsed, &TimerBase->VBlankTime);
	struct TimeRequest *tr, *trtmp;
	ForeachNodeSafe(&TimerBase->Lists[UNIT_VBLANK], tr, trtmp)
	{
		if ((tr->tr_time.tv_secs < TimerBase->Elapsed.tv_secs)
		 ||((tr->tr_time.tv_secs <= TimerBase->Elapsed.tv_secs)
		 && (tr->tr_time.tv_micro < TimerBase->Elapsed.tv_micro)))
		{
			
			Remove((struct Node *)tr);
			tr->tr_time.tv_secs = tr->tr_time.tv_micro = 0;
			tr->tr_node.io_Error = 0;
			ReplyMsg((struct Message *)tr);
			break;
		} 
	}

	ForeachNodeSafe(&TimerBase->Lists[UNIT_WAITUNTIL], tr, trtmp)
	{
		if ((tr->tr_time.tv_secs < TimerBase->Elapsed.tv_secs)
		 ||((tr->tr_time.tv_secs <= TimerBase->Elapsed.tv_secs)
		 && (tr->tr_time.tv_micro < TimerBase->Elapsed.tv_micro)))
		{
			
			Remove((struct Node *)tr);
			tr->tr_time.tv_secs = tr->tr_time.tv_micro = 0;
			tr->tr_node.io_Error = 0;
			ReplyMsg((struct Message *)tr);
			break;
		} 
	}
	return 0; // we return 0 so that Tick() can run, otherwise we would cut off Schedule()
}

__attribute__((no_instrument_function)) BOOL Timer1IRQServer(UINT32 number, istate* istate, TimerBase *TimerBase, APTR SysBase)
{
/* Have to think about this.
	struct EClockVal *tmp, tmp2;
	struct TimeRequest *tr, *trtmp;
	timer_ReadEClock(TimerBase, &tmp2);

	ForeachNodeSafe(&TimerBase->Lists[UNIT_ECLOCK], tr, trtmp)
	{
		tmp = (struct EClockVal *) &tr->tr_time;		
		if 	((tmp->ev_Lo < tmp2.ev_Lo) 
			|| ((tmp->ev_Lo <= tmp2.ev_Lo)
			&& (tmp->ev_Hi < tmp2.ev_Lo)))
		{		
			Remove((struct Node *)tr);
			tr->tr_time.tv_secs = tr->tr_time.tv_micro = 0;
			tr->tr_node.io_Error = 0;
			ReplyMsg((struct Message *)tr);
			break;
		}
	}

	ForeachNodeSafe(&TimerBase->Lists[UNIT_WAITECLOCK], tr, trtmp)
	{
		tmp = (struct EClockVal *) &tr->tr_time;		
		if 	((tmp->ev_Lo < tmp2.ev_Lo) 
			|| ((tmp->ev_Lo <= tmp2.ev_Lo)
			&& (tmp->ev_Hi < tmp2.ev_Lo)))
		{		
			Remove((struct Node *)tr);
			tr->tr_time.tv_secs = tr->tr_time.tv_micro = 0;
			tr->tr_node.io_Error = 0;
			ReplyMsg((struct Message *)tr);
			break;
		}
	}
	if ((IsListEmpty(&TimerBase->Lists[UNIT_WAITECLOCK]) && (IsListEmpty(&TimerBase->Lists[UNIT_WAITECLOCK]))
	{
		//DisableTimer1Int(); we dont use it anymore
	}
	*/
	return 1; // we return 1 noone else should use this timer!
}
