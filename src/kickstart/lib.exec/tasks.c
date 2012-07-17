#include "types.h"
#include "tasks.h"
#include "sysbase.h"
#include "context.h"
#include "exec_funcs.h"

void lib_Print_uart0(const char *s);
extern SysBase *g_SysBase;

#undef DEBUGTASKS

void lib_hexstrings(UINT32);

UINT32 interrupts_disable(void);
UINT32 interrupts_enable(void);
void interrupts_restore(UINT32 ipl);
UINT32 interrupts_read(void);
BOOL interrupts_disabled(void);

Task *lib_FindTask(SysBase *SysBase, STRPTR name)
{
  if (name == NULL) return SysBase->thisTask;
  Task *task=NULL;

  Disable();
  task = (Task *)FindName((List*)&SysBase->TaskReady, name);
  if (task == NULL) task = (Task *)FindName((List*)&SysBase->TaskWait, name);
  Enable();
  return task;
}

static void TaskRun(void)
{
	SysBase *SysBase = g_SysBase;
	Task *itsme = SysBase->thisTask;
	APTR arg = itsme->TaskArg;
	void (*f)(void *) = itsme->TaskFunc;
	interrupts_enable();
	f(arg);
	
	//DPrintF("Left Main\n");
	itsme->State = REMOVED;
	Schedule();
	for(;;); //Not reached
}

Task *lib_TaskCreate(SysBase *SysBase, char *name, APTR codeStart, APTR data, UINT32 stackSize, INT8 pri)
{
	Task *newTask = AllocVec(sizeof(Task), MEMF_FAST|MEMF_CLEAR);
	if (newTask==NULL) return NULL;

	newTask->Stack = AllocVec(stackSize, MEMF_FAST|MEMF_CLEAR);
	if (newTask->Stack == NULL) 
	{
		FreeVec(newTask);
		return NULL;
	}
	newTask->StackSize = stackSize;

	if (name == NULL) 	newTask->Node.ln_Name = "UnknownTask";
	else newTask->Node.ln_Name = name;
	newTask->Node.ln_Pri = pri;
	return AddTask(newTask, codeStart, NULL, data);
}

Task *lib_AddTask(SysBase *SysBase, Task *newTask, APTR codeStart, APTR finalPC, APTR data) 
{
	if (newTask == NULL) return NULL;
	#ifdef DEBUGTASKS
	DPrintF("[TaskCreate] Name: %s\n", name);
	DPrintF("[TaskCreate] stackSize: %x\n", stackSize);	
	DPrintF("[TaskCreate] Stack: %x\n", newTask->Stack);
	#endif
	
	newTask->Node.ln_Type = NT_TASK; //Perhaps PowerDOS wants to add this?!
	if (newTask->Node.ln_Name == NULL) 	newTask->Node.ln_Name = "UnknownTask";
	newTask->State = READY;
	newTask->CPU_Usage = 0;
	newTask->TDNestCnt = -1; // TaskSched allowed
	newTask->IDNestCnt = -1; // Ints allowed
	newTask->TaskArg = data;
	newTask->TaskFunc = codeStart;
	
	context_save(&newTask->SavedContext);
	context_set(&newTask->SavedContext, FADDR(TaskRun), (UINTPTR) newTask->Stack, newTask->StackSize);
	UINT32 ipl = interrupts_disable();
	newTask->SavedContext.ipl = interrupts_read();
	interrupts_restore(ipl);
	
	Enqueue(&SysBase->TaskReady,&newTask->Node);
	return newTask;
}

INT8 lib_AllocSignal(SysBase *SysBase, INT32 signalNum)
{
  struct Task *thisTask;
  UINT32 mask;
  UINT32 mask1 = 1,mask2 = 1;

  thisTask = (struct Task *)FindTask(NULL);

  mask = (UINT32)thisTask->SigAlloc;

  /* Setze Startwert */
  if((signalNum<0) || (signalNum>31)) signalNum = 31;
  mask1=mask2<<signalNum;			/* 0b000...001 << 31 = 0b100...000 */

  // Here we Check if this Signal is free (0b100..00 & 0b000..01)
  while (signalNum > -1)
  {
    if (!(mask1 & mask)) break;
    signalNum--;
    mask1 = mask2<<signalNum;
  }

  if (signalNum<0) return (INT8)-1;

  // delete recieved signals.....
  thisTask->SigAlloc  |= mask1;

  mask2 = ~mask1;
  thisTask->SigRecvd  &= mask2;
  thisTask->SigExcept &= mask2;
  thisTask->SigWait   &= mask2;

  return (INT8)signalNum;
}

void lib_FreeSignal(SysBase *SysBase, INT32 signalNum)
{
  if (signalNum != -1)
  {
    struct Task *task;
    task = (struct Task *)FindTask(NULL);
    task->SigAlloc &= ~(1<<signalNum);
  }
}

INT8 lib_SetTaskPri(SysBase *SysBase, struct Task *task, INT16 pri)
{
//	lib_Print_uart0("Change Pri\n");

	INT8 old;
//	lib_Print_uart0("[Change Pri] FindTask");
	if (task = NULL) task = FindTask(NULL);
//	lib_Print_uart0("[Change Pri] Forbid()");
	Forbid();
	old = task->Node.ln_Pri;
	task->Node.ln_Pri = (INT8)pri;

//	lib_Print_uart0("[Change Pri] Task-State:");
//	lib_hexstrings(task->State);
	
	if (task->State == READY)
	{
    	Remove(&task->Node);
    	Enqueue(&SysBase->TaskReady, &task->Node);
	}
	Permit();
//	lib_Print_uart0("[Change Pri] Schedule\n");

	if ((SysBase->TDNestCnt < 0) && (!(IsListEmpty(&SysBase->TaskReady)))) Schedule();
	return old;
}




