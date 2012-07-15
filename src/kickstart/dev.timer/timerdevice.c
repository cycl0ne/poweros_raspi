#include "timer_intern.h"
#include "exec_funcs.h"

struct TimerBase *timer_OpenDev(struct TimerBase *TimerBase, struct IORequest *ioreq, UINT32 unitNum, UINT32 flags)
{
	//DPrintF("[TimerDev] Open Unit: %d\n", unitNum);

    TimerBase->Device.dd_Library.lib_OpenCnt++;
    TimerBase->Device.dd_Library.lib_Flags &= ~LIBF_DELEXP;
    ioreq->io_Message.mn_Node.ln_Type = NT_REPLYMSG;

    switch(unitNum)
    {
	case UNIT_VBLANK:
	case UNIT_MICROHZ:
	case UNIT_WAITUNTIL:
	    ioreq->io_Error = 0;
	    ioreq->io_Unit = (struct Unit *)unitNum;
	    ioreq->io_Device = (struct Device *)TimerBase;
	    break;

	case UNIT_ECLOCK:
	case UNIT_WAITECLOCK:	
	default:
	    ioreq->io_Error = IOERR_OPENFAIL;
    }
	//DPrintF("[TimerDev] Open Unit: %d\n", unitNum);
	return(TimerBase);
}

APTR timer_CloseDev(struct TimerBase *TimerBase, struct IORequest *ioreq)
{
	TimerBase->Device.dd_Library.lib_OpenCnt--;
	if(!TimerBase->Device.dd_Library.lib_OpenCnt){}
	return(NULL);
}

UINT32 *timer_ExpungeDev(struct TimerBase *TimerBase)
{
	return (UINT32 *)FALSE;
}

UINT32 timer_ExtFuncDev(void)
{
	return (0);
}



