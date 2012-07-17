#include "timer_intern.h"
#include "exec_funcs.h"

INT32 timer_CmpTime(struct TimerBase *TimerBase, struct TimeVal *src, struct TimeVal *dst);
void timer_AddTime(struct TimerBase *TimerBase, struct TimeVal *src, struct TimeVal *dst);
void timer_SubTime(struct TimerBase *TimerBase, struct TimeVal *src, struct TimeVal *dst);

static void SetSysTime(TimerBase *TimerBase, struct TimeRequest *ioreq)
{
    Disable();
	TimerBase->CurrentTime.tv_micro= ioreq->tr_time.tv_micro;
	TimerBase->CurrentTime.tv_secs = ioreq->tr_time.tv_secs;
	Enable();
}

static void GetSysTime(TimerBase *TimerBase, struct TimeRequest *ioreq)
{
	ioreq->tr_time.tv_micro = TimerBase->CurrentTime.tv_micro;
	ioreq->tr_time.tv_secs  = TimerBase->CurrentTime.tv_secs;
}

static void AddWaiter(TimerBase *TimerBase, struct MinList *list, struct TimeRequest *iotr)
{
	struct TimeRequest *tr;
	BOOL added = FALSE;

	tr = (struct TimeRequest *)list->mlh_Head;

	while(tr->tr_node.io_Message.mn_Node.ln_Succ != NULL)
    {
		/* If the time in the new request is less than the next request */
		if(timer_CmpTime(TimerBase, &tr->tr_time, &iotr->tr_time) < 0)
		{
	        /* Add the node before the next request */
			Insert((struct List *)list,(struct Node *)iotr,tr->tr_node.io_Message.mn_Node.ln_Pred);		
	        added = TRUE;
	        break;
	    }
		tr = (struct TimeRequest *)tr->tr_node.io_Message.mn_Node.ln_Succ;
	}
	if(!added) AddTail((struct List *)list, (struct Node *)iotr);
//	ForeachNode(list, tr)
//	{
//		DPrintF("%u.%u\n", tr->tr_time.tv_secs, tr->tr_time.tv_micro);
//	}
}

static void AddRequest(struct TimerBase *TimerBase, struct TimeRequest *ioreq)
{
	UINT32 unitNum;
	unitNum = (UINT32)ioreq->tr_node.io_Unit;
	
	switch(unitNum)
	{
	case UNIT_WAITUNTIL :
		Disable();
		if(timer_CmpTime(TimerBase, &TimerBase->CurrentTime, &ioreq->tr_time) <= 0)
		{
			Enable();
			ioreq->tr_time.tv_secs = ioreq->tr_time.tv_micro = 0;
			ioreq->tr_node.io_Error = 0;
			ReplyMsg((struct Message *)ioreq);
		}
		else
		{
			/* Ok, we add this to the list */
			AddWaiter(TimerBase, &TimerBase->Lists[UNIT_WAITUNTIL], ioreq);
			Enable();
			ioreq->tr_node.io_Flags &= ~IOF_QUICK;
		}
		break;

	case UNIT_MICROHZ:
	case UNIT_VBLANK :
	    Disable();
		//DPrintF("[Timer.device] AddTime\n");
	    timer_AddTime(TimerBase, &TimerBase->Elapsed, &ioreq->tr_time);
		//DPrintF("[Timer.device] AddWaiter\n");
        AddWaiter(TimerBase, &TimerBase->Lists[UNIT_VBLANK], ioreq);
	    Enable();
		//DPrintF("[Timer.device] return\n");
	    ioreq->tr_node.io_Flags &= ~IOF_QUICK;
		break;
	case UNIT_ECLOCK :
	case UNIT_WAITECLOCK:
		break;
	default:
		ioreq->tr_node.io_Error = IOERR_OPENFAIL;
		break;
	}
}


void timer_BeginIO(TimerBase *TimerBase, struct IORequest *ioreq)
{
	BOOL Msg = TRUE;
    ioreq->io_Message.mn_Node.ln_Type = NT_MESSAGE;
    ioreq->io_Error = 0;

	//DPrintF("[Timer.device] BeginIO\n");
	switch(ioreq->io_Command) 
	{
		case TR_ADDREQUEST:
			AddRequest(TimerBase, (struct TimeRequest *) ioreq);
			break;
		case TR_GETSYSTIME:
			GetSysTime(TimerBase, (struct TimeRequest *) ioreq);
			if (!(ioreq->io_Flags & IOF_QUICK)) ReplyMsg((struct Message *)ioreq);
			break;
		case TR_SETSYSTIME:
			SetSysTime(TimerBase, (struct TimeRequest *) ioreq);
			if (!(ioreq->io_Flags & IOF_QUICK)) ReplyMsg((struct Message *)ioreq);
			break;
		default:
	        ioreq->io_Error = IOERR_NOCMD;
			ReplyMsg((struct Message *)ioreq);
			break;
	}
}

void timer_AbortIO(TimerBase *TimerBase, struct IORequest *ioreq)
{
	if(ioreq->io_Message.mn_Node.ln_Type != NT_REPLYMSG)
    {
		Remove((struct Node *)ioreq);
		ReplyMsg((struct Message *)ioreq);
	}
}

