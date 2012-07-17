#include "types.h"
#include "list.h"
#include "ports.h"
#include "sysbase.h"
#include "device.h"
#include "io.h"

#include "exec_funcs.h"


struct Message *lib_GetMsg(SysBase *SysBase, struct MsgPort *msgPort)
{
   struct Message *ret;
   Disable();
   ret = (struct Message *)RemHead(&msgPort->mp_MsgList);
   Enable();
   return ret;
}

void lib_PutMsg(SysBase *SysBase, struct MsgPort *msgPort, struct Message *msg)
{
	Disable();
	msg->mn_Node.ln_Type = NT_MESSAGE;
	AddTail(&msgPort->mp_MsgList, &msg->mn_Node);
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
	Enable();
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
		Disable();
		msg->mn_Node.ln_Type=NT_REPLYMSG;
		AddTail(&msgPort->mp_MsgList,&msg->mn_Node);

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
		Enable();
	}
}

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

UINT32 lib_SetSignal(SysBase *SysBase, UINT32 newSignals, UINT32 signalSet)
{
  UINT32 old;
  struct Task *task;

  task = (struct Task *)FindTask(NULL);
  old = task->SigRecvd;
  Disable();
  task->SigRecvd = (old&~signalSet)|(signalSet&newSignals);
  Enable();
  return old;
}

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






