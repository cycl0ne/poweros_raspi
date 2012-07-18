#include "types.h"
#include "list.h"
#include "ports.h"
#include "sysbase.h"
#include "device.h"
#include "io.h"

#include "exec_funcs.h"


struct Message *lib_GetMsg(SysBase *SysBase, struct MsgPort *msgPort)
{
	UINT32 ipl;
	struct Message *ret;
	ipl = Disable();
	ret = (struct Message *)RemHead(&msgPort->mp_MsgList);
	Enable(ipl);
	return ret;
}

void lib_PutMsg(SysBase *SysBase, struct MsgPort *msgPort, struct Message *msg)
{
	UINT32 ipl;
	ipl = Disable();
	msg->mn_Node.ln_Type = NT_MESSAGE;
	AddTail(&msgPort->mp_MsgList, &msg->mn_Node);
	Enable(ipl);
	if (msgPort->mp_SigTask)
	{
		switch(msgPort->mp_Flags&PA_MASK)
		{
			case PA_SIGNAL:
				Signal(msgPort->mp_SigTask, 1 << msgPort->mp_SigBit);
				break;
			case PA_SOFTINT:
				// This is wrong but.. at the moment it should do ;)
				Signal(msgPort->mp_SigTask, 1 << msgPort->mp_SigBit);
				break;
			case PA_IGNORE:
			case PA_UNKNOWN:
			default:
			break;
		}
	}
}

void lib_ReplyMsg(SysBase *SysBase, struct Message *msg)
{
   struct MsgPort *msgPort;
   msgPort = msg->mn_ReplyPort;

	if (msgPort==NULL)
	{
		msg->mn_Node.ln_Type=NT_FREEMSG;
	} else
	{
		UINT32 ipl = Disable();
		msg->mn_Node.ln_Type=NT_REPLYMSG;
		AddTail(&msgPort->mp_MsgList,&msg->mn_Node);
		Enable(ipl);
		if (msgPort->mp_SigTask)
		{
			switch(msgPort->mp_Flags&PA_MASK)
			{
			case PA_SIGNAL:
				//DPrintF("Signal %x , Bit: %x\n",msgPort->mp_SigTask, 1 << msgPort->mp_SigBit);
				Signal(msgPort->mp_SigTask, 1 << msgPort->mp_SigBit);
				break;
			case PA_SOFTINT:
				// This is wrong but.. at the moment it should do ;)
				Signal(msgPort->mp_SigTask, 1 << msgPort->mp_SigBit);
				break;
			case PA_IGNORE:
			case PA_UNKNOWN:
			default:
				break;
			}
		}
	}
}

UINT32 lib_SetSignal(SysBase *SysBase, UINT32 newSignals, UINT32 signalSet)
{
  UINT32 old;
  struct Task *task;

  task = (struct Task *)FindTask(NULL);
  old = task->SigRecvd;
  UINT32 ipl = Disable();
  task->SigRecvd = (old&~signalSet)|(signalSet&newSignals);
  Enable(ipl);
  return old;
}

UINT32 interrupts_disable(void);
UINT32 interrupts_enable(void);
void interrupts_restore(UINT32 ipl);
#include "context.h"

UINT32 WaitPrepare(Task *task)
{
	UINT32 ipl = interrupts_disable();
	if (task)
	{
		return ipl;
	}
	interrupts_restore(ipl);
	return 0;
}

void WaitFinish(UINT32 ipl)
{
	interrupts_restore(ipl);
}

void WaitTask(SysBase *SysBase, struct Task *Task)
{
	if (!context_save(&Task->WaitContext))
	{
		return;
	}
//	Task->CPU_Usage++;
	Task->TDNestCnt = SysBase->TDNestCnt;
	Task->IDNestCnt = SysBase->IDNestCnt;
//	if (Task->Switch) Task->Switch(SysBase);
	AddHead(&SysBase->TaskWait, &Task->Node);
	Task->State = WAIT;
	SysBase->thisTask = NULL;
	Schedule();
	DPrintF("Never reached\n");
}

UINT32 lib_Wait(SysBase *SysBase, UINT32 signalSet)
{
	UINT32 rcvd, ipl;
	struct Task *thisTask;

	thisTask = (struct Task *)FindTask(NULL);
	while(!(thisTask->SigRecvd & signalSet))
	{
		ipl = WaitPrepare(thisTask);
		thisTask->SigWait=signalSet;
		WaitTask(SysBase, thisTask);
		WaitFinish(ipl);
	}
	rcvd=thisTask->SigRecvd & signalSet;
	thisTask->SigRecvd &= ~(signalSet);
	return rcvd;
}

void WakeupTask(SysBase *SysBase, struct Task *Task, UINT32 signal)
{
	UINT32 ipl = interrupts_disable();
	// Remove him from the Waitqueue
	Remove(&Task->Node);
	Task->SavedContext = Task->WaitContext;
    Task->SigRecvd|=signal;
	Task->State = READY;
//	Task->CPU_Usage++;
//	Task->TDNestCnt = SysBase->TDNestCnt;
//	Task->IDNestCnt = SysBase->IDNestCnt;
	Enqueue(&SysBase->TaskReady, &Task->Node);
//	if (Task->Switch) Task->Switch(SysBase);
	interrupts_restore(ipl);
}

void lib_Signal(SysBase *SysBase, struct Task *task, UINT32 signalSet)
{
	WakeupTask(SysBase, task, signalSet);
}

#if 0
UINT32 lib_Wait(SysBase *SysBase, UINT32 signalSet)
{
  UINT32 rcvd;
  struct Task *thisTask;

  thisTask = (struct Task *)FindTask(NULL);
  Disable();
  while(!(thisTask->SigRecvd & signalSet))
  {
    thisTask->SigWait=signalSet;
    //thisTask->TDNestCnt = SysBase->TDNestCnt;
	//SysBase->TDNestCnt = -1;
    thisTask->State = WAIT;
    //AddTail(&SysBase->TaskWait,&thisTask->Node);
    Schedule();
    //Disable();
    //SysBase->TDNestCnt=thisTask->TDNestCnt;
  }
  rcvd=thisTask->SigRecvd & signalSet;
  /* And clear them. */
  thisTask->SigRecvd &= ~(signalSet);

  /* All done. */
  Enable();
  return rcvd;
}

#endif

#if 0
void lib_Signal(SysBase *SysBase, struct Task *task, UINT32 signalSet)
{
	Disable();
    task->SigRecvd|=signalSet;

    if(task->SigExcept&task->SigRecvd)
    {
        task->Flags|=TF_EXCEPT;
        if (RUN == task->State)
        {
            Schedule();
            Enable();
            return;
        }
    }

    if ((task->State == WAIT)&&(task->SigRecvd&(task->SigWait|task->SigExcept)))
    {
		task->State = READY;
        Remove(&task->Node);
        Enqueue(&SysBase->TaskReady, &task->Node);
    }
    struct Task *tmp= FindTask(NULL);
    if (task->Node.ln_Pri > tmp->Node.ln_Pri)
    {
        Schedule();
    }
	Enable();
}
#endif


