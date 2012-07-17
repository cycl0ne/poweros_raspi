#include "types.h"
#include "list.h"
#include "ports.h"
#include "sysbase.h"

#include "exec_funcs.h"

void lib_AddPort(SysBase *SysBase, struct MsgPort *msgPort)
{
   msgPort->mp_Node.ln_Type = NT_MSGPORT;
   NewListType(&msgPort->mp_MsgList,NT_MSGPORT);
   Forbid();
   Enqueue(&SysBase->PortList, &msgPort->mp_Node);
   Permit();
}

struct MsgPort *lib_FindPort(SysBase *SysBase, STRPTR name)
{
   return (struct MsgPort *)FindName(&SysBase->PortList, name);
}

void lib_RemPort(SysBase *SysBase, struct MsgPort *msgPort)
{
   Forbid();
   Remove(&msgPort->mp_Node);
   Permit();
}

struct Message *lib_WaitPort(SysBase *SysBase, struct MsgPort *msgPort)
{
	while (IsListEmpty(&msgPort->mp_MsgList))
  	{
  		Wait(1<<msgPort->mp_SigBit);
  	}
  	return (struct Message *) msgPort->mp_MsgList.lh_Head;
}

struct MsgPort *lib_CreateMsgPort(SysBase *SysBase)
{
	struct MsgPort *ret=NULL;

   	ret = (struct MsgPort *)AllocVec(sizeof(struct MsgPort), MEMF_FAST|MEMF_CLEAR); //MEMF_PUBLIC|MEMF_CLEAR);
   	if (NULL != ret)
   	{
     	INT8 sb;
     	sb = AllocSignal(-1);
     	if (sb != -1)
     	{
       		ret->mp_SigBit = sb;
       		ret->mp_Flags  = PA_SIGNAL;
       		ret->mp_Node.ln_Type = NT_MSGPORT;
       		ret->mp_SigTask = (struct Task *)FindTask(NULL);
       		DPrintF("MsgPort Task: %x [%s]\n",ret->mp_SigTask, ret->mp_SigTask->Node.ln_Name);
       		NewListType(&ret->mp_MsgList, NT_MSGPORT);
     	} else
     	{
       		FreeVec(ret);
       		ret = NULL;
     	}
   	}
	return ret;
}

void lib_DeleteMsgPort(SysBase *SysBase, struct MsgPort *msgPort)
{
	if (NULL != msgPort)
  	{
    	FreeSignal(msgPort->mp_SigBit);
    	FreeVec(msgPort);
  	}
}


