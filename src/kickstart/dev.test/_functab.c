#include "testdev.h"
#include "timer.h"
#include "resident.h"

#include "exec_funcs.h"

#define DEVICE_VERSION_STRING "\0$VER: testdev.device 0.1 (__DATE__)\r\n";
#define DEVICE_VERSION 0
#define DEVICE_REVISION 1

#define TDEV_UNIT_MAX	1
#define _TDTASK_STACK_ 0x4000

static const char name[] = "testdev.device";
static const char version[] = DEVICE_VERSION_STRING
static const char EndResident;

APTR tdev_OpenDev(struct TestDevBase *TestDevBase, struct IORequest *ioreq, UINT32 unitnum, UINT32 flags);
APTR tdev_CloseDev(struct TestDevBase *TestDevBase, struct IORequest *ioreq);
APTR tdev_ExpungeDev(struct TestDevBase *TestDevBase);
APTR tdev_ExtFuncDev(struct TestDevBase *TestDevBase);
void tdev_BeginIO(TestDevBase *TestDevBase, struct IORequest *ioreq);
void tdev_AbortIO(TestDevBase *tdev_, struct IORequest *ioreq);
static struct TestDevBase *tdev_Init(struct TestDevBase *TestDevBase, UINT32 *segList, APTR SysBase);
BOOL tdev_PerformIO(struct TestDevBase *TestDevBase, struct IORequest *ior);

static APTR FuncTab[] = 
{
	(void(*)) tdev_OpenDev,
	(void(*)) tdev_CloseDev,
	(void(*)) tdev_ExpungeDev,
	(void(*)) tdev_ExtFuncDev,

	(void(*)) tdev_BeginIO,
	(void(*)) tdev_AbortIO,
	(APTR) ((UINT32)-1)
};

static const APTR InitTab[4]=
{
	(APTR)sizeof(struct TestDevBase),
	(APTR)FuncTab,
	(APTR)NULL,
	(APTR)tdev_Init
};

static const struct Resident ROMTag = 
{
	RTC_MATCHWORD,
	(struct Resident *)&ROMTag,
	(APTR)&EndResident,
	RTF_AUTOINIT|RTF_COLDSTART,
	DEVICE_VERSION,
	NT_DEVICE,
	30,
	(char *)name,
	(char*)&version[7],
	0,
	&InitTab
};

static void dev_Task(struct TestDevBase *TestDevBase) 
{
	APTR SysBase = TestDevBase->SysBase;
	
	DPrintF("[TestDevTask] started (%x)\n", TestDevBase);
	
	TestDevBase->TaskPort	= CreateMsgPort();
	TestDevBase->TimerPort	= CreateMsgPort();

	// Here you should abort correctly and give resources back (DeleteMsgPort())
	if (NULL == TestDevBase->TaskPort) DPrintF("[TestDevTask] CreateMsgPort(TaskPort) failed\n");
	if (NULL == TestDevBase->TimerPort) DPrintF("[TestDevTask] CreateMsgPort(TimerPort) failed\n");

	TestDevBase->Timer_Unit = CreateIORequest(TestDevBase->TimerPort, sizeof(struct TimeRequest));

	// Same here, give the Ports back.
	if (NULL == TestDevBase->Timer_Unit) DPrintF("[TestDevTask] CreateIORequest(Timer_Unit) failed\n");
	
	if (0 != OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)TestDevBase->Timer_Unit, 0)) 
	{
		// Release all Resources, we failed.
		DPrintF("[TestDevTask] CreateIORequest(Timer_Unit) failed\n");
		return;
	}

	UINT32 signal = 0;
	UINT32 signalTask = (1 << TestDevBase->TaskPort->mp_SigBit);
	UINT32 signalTime = (1 << TestDevBase->TimerPort->mp_SigBit);
	UINT32 waitSignal = signalTask | signalTime;

	while(!TestDevBase->Flags & F_SHUTDOWN)
	{
		signal = 0;
		TestDevBase->Timer_Unit->tr_node.io_Command	= TR_ADDREQUEST;
		TestDevBase->Timer_Unit->tr_time.tv_secs	= 2;
		TestDevBase->Timer_Unit->tr_time.tv_micro	= 0;
		SendIO((struct IORequest *)TestDevBase->Timer_Unit);
		signal = Wait(waitSignal);
		
		if(signal&signalTask)
		{
			struct IORequest *ior = (struct IORequest *)GetMsg(TestDevBase->TaskPort);
			while(NULL != ior)
			{
				if (tdev_PerformIO(TestDevBase, ior)) ReplyMsg((struct Message *)ior);
			}
		} else
		{
			// its the timer calling us !
			TestDevBase->Timer++;
			if (TestDevBase->Timer == 5) DPrintF("[TestDevTask] Timer called us 5 Times\n");
		}
	}

	// Shutting down, give all Resources back! (TODO)
}

static struct TestDevBase *tdev_Init(struct TestDevBase *TestDevBase, UINT32 *segList, APTR SysBase)
{
	TestDevBase->Device.dd_Library.lib_OpenCnt = 0;
	TestDevBase->Device.dd_Library.lib_Node.ln_Pri = 0;
	TestDevBase->Device.dd_Library.lib_Node.ln_Type = NT_DEVICE;
	TestDevBase->Device.dd_Library.lib_Node.ln_Name = (STRPTR)name;
	TestDevBase->Device.dd_Library.lib_Version = DEVICE_VERSION;
	TestDevBase->Device.dd_Library.lib_Revision = DEVICE_REVISION;
	TestDevBase->Device.dd_Library.lib_IDString = (STRPTR)&version[7];
	
	TestDevBase->SysBase	= SysBase;
	TestDevBase->Flags		= 0;
	TestDevBase->Timer		= 0;
	// Normaly you should trigger the Task to start off, but this is the last thing to activate, so its ok.
	TestDevBase->Task = TaskCreate(name, (char *)dev_Task, TestDevBase, _TDTASK_STACK_, 10);	
}

static const char EndResident = 0;
