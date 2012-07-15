#include "types.h"
#include "tasks.h"
#include "sysbase.h"
#include "context.h"
#include "exec_funcs.h"

extern SysBase *g_SysBase;

UINT32 interrupts_disable(void);
UINT32 interrupts_enable(void);
void interrupts_restore(UINT32 ipl);
UINT32 interrupts_read(void);
BOOL interrupts_disabled(void);
void lib_Print_uart0(const char *s);

static void MakeTaskReady(SysBase *SysBase, Task *Task)
{
	if (Task) {
		Task->State = READY;
		Task->CPU_Usage++;
		Task->TDNestCnt = SysBase->TDNestCnt;
		if (Task->Switch) Task->Switch(SysBase);
		Enqueue(&SysBase->TaskReady, &Task->Node);
	}
}

static void MakeTaskWait(SysBase *SysBase, Task *Task)
{
	if (Task) {
		Task->CPU_Usage++;
		Task->TDNestCnt = SysBase->TDNestCnt;
		if (Task->Switch) Task->Switch(SysBase);
		Enqueue(&SysBase->TaskWait, &Task->Node);
	}
}

static void MakeTaskRun(SysBase *SysBase, Task *Task)
{
	if (Task) {
		Task->State = RUN;
		SysBase->TDNestCnt = Task->TDNestCnt;
		if (Task->Launch) Task->Launch(SysBase);
	}	
}

static void MakeTaskDestroy(SysBase *SysBase, Task *Task)
{
	// Here do some Garbage Collection
}
extern APTR supervisor_sp;

void before_thread_runs_arch(Task *Task)
{
	UINT8 *stck;
	
	stck = (UINT8 *)&Task->Stack[Task->StackSize - SP_DELTA];
	supervisor_sp = (APTR) stck;
}

void lib_Dispatch(void)
{
//	lib_Print_uart0("[DISPATCH]\n");
	SysBase *SysBase = g_SysBase;
	Task *Task = SysBase->thisTask;

	if (Task) {
		switch(Task->State) {
		case RUN:
			MakeTaskReady(SysBase, Task);
			break;
		case REMOVED:
			MakeTaskDestroy(SysBase, Task);
			break;
		case WAIT:
			MakeTaskWait(SysBase, Task);
			break;
		case DEAD:
		default:
			{
			DPrintF("[DEAD/Unknown] (%s)(%x)\n", Task->Node.ln_Name, Task->State);
			for(;;);
			break;
			}
		}
		Task = NULL;
	}
	Task = (struct Task *)RemHead(&SysBase->TaskReady);
	MakeTaskRun(SysBase, Task);
	SysBase->thisTask = Task;
	before_thread_runs_arch(Task);
	context_restore(&Task->SavedContext);
}

void lib_Schedule(SysBase *SysBase)
{
//	lib_Print_uart0("[SCHEDULE]\n");
	if ((SysBase->TDNestCnt >= 0) || (IsListEmpty(&SysBase->TaskReady))) return;
	volatile UINT32 ipl = interrupts_disable();

	Task *this = SysBase->thisTask;

	if (SysBase->thisTask) 
	{
		if (!context_save(&this->SavedContext)) 
		{
			//lib_Print_uart0("Coming Home\n");
			interrupts_restore(this->SavedContext.ipl);
			//lib_Print_uart0("Leaving Sched\n");
			return;
		}
		this->SavedContext.ipl = ipl;
	}
	context_save(&SysBase->CPU_Context);
	context_set(&SysBase->CPU_Context, FADDR(lib_Dispatch), SysBase->CPU_Stack, _CPUSTACK_);
	context_restore(&SysBase->CPU_Context);
}

