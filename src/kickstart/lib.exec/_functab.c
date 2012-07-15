#include "exec_proto_lib.h"
#include "exec_funcs.h"

#define _IDLE_TASK_STACK_ 4096
#define _EXEC_TASK_STACK_ 4096*4

APTR FuncTab[] =
{
	(void(*)) lib_OpenLib,
	(void(*)) lib_CloseLib,
	(void(*)) lib_ExpungeLib,
	(void(*)) lib_ExtFuncLib,
	
	(void(*)) lib_Permit,
	(void(*)) lib_Forbid,
	(void(*)) lib_Enable,
	(void(*)) lib_Disable,

	(void(*)) lib_NewList,
	(void(*)) lib_Enqueue,
	(void(*)) lib_FindName,
	(void(*)) lib_RemTail,
	(void(*)) lib_AddTail,
	(void(*)) lib_RemHead,
	(void(*)) lib_AddHead,
	(void(*)) lib_Remove,
	(void(*)) lib_Insert,
	(void(*)) lib_NewListType,

	(void(*)) lib_FindTask,
	(void(*)) lib_AddTask,
	(void(*)) lib_AddMemList,
	(void(*)) lib_Allocate,
	(void(*)) lib_Deallocate,
	(void(*)) lib_AllocVec,
	(void(*)) lib_FreeVec,
	
	(void(*)) lib_Schedule,
	(void(*)) NULL,  // Dispatch

	(void(*)) lib_GetMsg,
	(void(*)) lib_PutMsg,
	(void(*)) lib_ReplyMsg,
	(void(*)) lib_Signal,
	(void(*)) lib_Wait,

	(void(*)) lib_OpenDevice,
	(void(*)) lib_CloseDevice,
	(void(*)) lib_RemDevice,
	(void(*)) lib_AddDevice,

	(void(*)) lib_AbortIO,
	(void(*)) lib_CreateIORequest,
	(void(*)) lib_DeleteIORequest,
	(void(*)) lib_CheckIO,
	(void(*)) lib_DoIO,
	(void(*)) lib_SendIO,
	(void(*)) lib_WaitIO,

	(void(*)) lib_AddPort,
	(void(*)) lib_FindPort,
	(void(*)) lib_RemPort,
	(void(*)) lib_WaitPort,
	(void(*)) lib_CreateMsgPort,
	(void(*)) lib_DeleteMsgPort,
	
	(void(*)) lib_AllocSignal,
	(void(*)) lib_FreeSignal,

	(void(*)) lib_OpenLibrary,
	(void(*)) lib_CloseLibrary,
	(void(*)) lib_AddLibrary,
	(void(*)) lib_RemLibrary,
	(void(*)) lib_SumLibrary,
	(void(*)) lib_Alert,
	(void(*)) lib_MakeLibrary,
	(void(*)) lib_RomTagScanner,
	(void(*)) lib_InitResident,
	(void(*)) lib_MakeFunctions,
	(void(*)) lib_SetTaskPri,

	(void(*)) lib_SNPrintF,
	(void(*)) lib_VPrintF,
	(void(*)) lib_VSNPrintF,
	(void(*)) lib_MemSet,
	(void(*)) lib_CopyMem,

	(void(*)) lib_SetSignal,


	(void(*)) lib_AddIntServer,
	(void(*)) lib_RemIntServer,
	(void(*)) lib_CreateIntServer,
	(void(*)) lib_AddExcServer,
	(void(*)) lib_RemExcServer,

	(void(*)) lib_DPrintF,
		
	(APTR) ((INT32)-1)
};

static UINT32 intern_CountFunc(APTR functionArray);
static void IntMakeFunctions(APTR target, APTR functionArray);
void lib_hexstrings ( unsigned int d );
void lib_Print_uart0(const char *s);
void _InitExcServer(SysBase *SysBase);
#define FUNCTAB_DEBUG
#ifdef FUNCTAB_DEBUG
	#define DEBUG(fmt, args...) DPrintF("[%s:%d] " fmt, __PRETTY_FUNCTION__, __LINE__ , ##args)
#else
	#define DEBUG(fmt, args...) do {} while(0)
#endif

static void lib_Idle(APTR SysBase) 
{
	//SysBase *SysBase = g_SysBase;
	DPrintF("[IDLE] Started\n");
	SetTaskPri(NULL, -125);
	for(;;);
}

Task *TaskCreate(SysBase *SysBase, char *name, APTR codeStart, APTR data, UINT32 stackSize, INT8 pri);
void lib_ExecTask(struct SysBase *SysBase);

SysBase *CreateSysBase(struct MemHeader *memStart)
{
	struct SysBase *SysBase;
//	lib_Print_uart0("CountFunc:");
	UINT32 negsize = intern_CountFunc(&FuncTab);
//	lib_hexstrings((UINT32) negsize);
//	lib_Print_uart0("Allocate:\n");
	SysBase = lib_Allocate(NULL, memStart, negsize+sizeof(struct SysBase));
//	lib_hexstrings((UINT32) SysBase);

	if (SysBase == NULL) 
	{
		lib_Print_uart0("ERROR, No Memory for SysBase");
		for(;;);
	}
	// Create the Data field below Jumptable
	SysBase = (struct SysBase *)((UINT32)SysBase + negsize);

//	lib_Print_uart0("SysBase:");
//	lib_hexstrings((UINT32) SysBase);
//	lib_hexstrings((UINT32) sizeof(struct SysBase));

	IntMakeFunctions(SysBase, &FuncTab);
	UINT32 tmp = ((UINT32 *)SysBase)[-1];
	lib_hexstrings((UINT32) tmp);
	lib_hexstrings((UINT32) ((UINT32 *)SysBase)[-2]);
	lib_hexstrings((UINT32) lib_OpenLib);
	lib_hexstrings((UINT32) lib_CloseLib);

	// FROM NOW ON YOU CAN USE JUMPTABLE
	SysBase->IDNestCnt = 1;
   	NewListType(&SysBase->TaskReady		,NT_TASK);
   	NewListType(&SysBase->TaskWait		,NT_TASK);  	
   	NewListType(&SysBase->PortList		,NT_MSGPORT);
   	NewListType(&SysBase->MemList		,NT_MEMORY);
   	NewListType(&SysBase->DevList		,NT_DEVICE);
   	NewListType(&SysBase->LibList		,NT_LIBRARY);
   	NewListType(&SysBase->SemaphoreList	,NT_SEMAPHORE);
   	NewListType(&SysBase->ResourceList	,NT_RESOURCE);
    NewListType(&SysBase->ResidentList	,NT_RESIDENT);

	DEBUG("Newlist: Done\n");

	for (int i=0 ; i<8; i++) NewListType((struct List *)&SysBase->ExcServer[i], NT_INTERRUPT);
	for (int i=0 ; i<NR_IRQS; i++) NewListType((struct List *)&SysBase->IRQServer[i], NT_INTERRUPT);

	DEBUG("Newlist: Done\n");
	
	Enqueue(&SysBase->MemList, &memStart->mh_Node);

   	SysBase->LibNode.lib_Node.ln_Pri  = -127;
   	SysBase->LibNode.lib_Node.ln_Type = NT_LIBRARY;
   	SysBase->LibNode.lib_Version      = 0;
   	SysBase->LibNode.lib_Revision     = 1;
   	SysBase->LibNode.lib_Node.ln_Name = "exec.library";
   	SysBase->LibNode.lib_IDString     = "\0$VER: exec.library 0.1 (__DATE__)\r\n";
   	SysBase->LibNode.lib_PosSize      = sizeof(struct SysBase);
   	SysBase->LibNode.lib_NegSize      = negsize;
   	SysBase->LibNode.lib_OpenCnt      = 1;

	// And now you can use SysBase Values!

	DEBUG("AlocCPU Stack\n");
	
	SysBase->CPU_Stack = AllocVec(_CPUSTACK_,MEMF_FAST|MEMF_CLEAR);

	DEBUG("Init EXCeptions Vector\n");
	_InitExcServer(SysBase);

	DEBUG("Enqueue Library\n");	
    // Enqueue our new build Library to our Library Manager.
   	Enqueue(&SysBase->LibList, &SysBase->LibNode.lib_Node);
	DEBUG("Done, return SysBase: %x\n", SysBase);

	DPrintF("[INIT] Create Idle Task\n");
	Task *task1 = TaskCreate(SysBase, "idle", lib_Idle, SysBase, _IDLE_TASK_STACK_, 128); 
	Task *task2 = TaskCreate(SysBase, "ExecTask", lib_ExecTask, SysBase, _EXEC_TASK_STACK_, 100);

	if (RomTagScanner((APTR)0x8000, (APTR)0x180000) == FALSE)
	{
		DPrintF("[INIT] ROMTAG Scanner Failed\n");
		for(;;);
	}

	struct ResidentNode *res;
	DPrintF("[INIT] RTF_SINGLETASK\n");
    ForeachNode(&SysBase->ResidentList, res)
    {
        if (res->rn_Resident->rt_Flags & RTF_SINGLETASK) 
        {
            DPrintF("(SINGLETASK)InitResident %s (%x)\n", res->rn_Resident->rt_Name,res->rn_Resident);    
            if (InitResident(res->rn_Resident, NULL)== NULL) 
            {
				DPrintF("[INIT] ROMTAG Scanner Failed (SingleTask)\n");
				DPrintF("on Resident [%x] name: %s\n", res->rn_Resident, res->rn_Resident->rt_Name);
				for(;;);
            }
        }
    }
	return SysBase;
}

static void IntMakeFunctions(APTR target, APTR functionArray)
{
	INT32 n = 1;
	APTR vector;
	
	void **fp = (void **)functionArray;
	while(*fp != (void*)-1)
	{
		vector = (APTR)((UINT32)target-n*4);
		*(UINT32 *)(((UINT32)vector)) = (UINT32) *fp;
		fp++;
		n++;
	}
}

static UINT32 intern_CountFunc(APTR functionArray)
{
	UINT32 n=0;
	void **fp=(void **)functionArray;

	/* -1 terminates the array */
	while(*fp!=(void *)-1)
	{
		fp++;
		n++;
	}
	return n*4;
}

