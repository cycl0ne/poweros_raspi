#include "timer_intern.h"

#undef SysBase
#include "sysbase.h"
#include "resident.h"

// Include our Protos
#include "exec_funcs.h"

//#define TIMERNAME "timer.device"


static char DevName[] = "timer.device";
static char Version[] = "\0$VER: timer.device 0.1 (__DATE__)\r\n";
#define TIMER_INT_PRI 0

struct TimerBase *timer_OpenDev(struct TimerBase *TimerBase, struct IORequest *ioreq, UINT32 unitNum, UINT32 flags);
APTR timer_CloseDev(struct TimerBase *TimerBase, struct IORequest *ioreq);
UINT32 *timer_ExpungeDev(struct TimerBase *TimerBase);
UINT32 timer_ExtFuncDev(void);
void timer_BeginIO(TimerBase *TimerBase, struct IORequest *ioreq);
void timer_AbortIO(TimerBase *TimerBase, struct IORequest *ioreq);
INT32 timer_CmpTime(struct TimerBase *TimerBase, struct TimeVal *src, struct TimeVal *dst);
void timer_AddTime(struct TimerBase *TimerBase, struct TimeVal *src, struct TimeVal *dst);
void timer_SubTime(struct TimerBase *TimerBase, struct TimeVal *src, struct TimeVal *dst);
void timer_GetSysTime(struct TimerBase *TimerBase, struct TimeVal *src);
UINT32 timer_ReadEClock(struct TimerBase *TimerBase, struct EClockVal *src);
__attribute__((no_instrument_function)) BOOL TimerIRQServer(UINT32 number, istate* istate, TimerBase *TimerBase, APTR SysBase);


APTR timer_FuncTab[] = 
{
 (void(*)) timer_OpenDev,
 (void(*)) timer_CloseDev,
 (void(*)) timer_ExpungeDev,
 (void(*)) timer_ExtFuncDev,

 (void(*)) timer_BeginIO,
 (void(*)) timer_AbortIO,

 (void(*)) timer_AddTime,
 (void(*)) timer_CmpTime,
 (void(*)) timer_GetSysTime,
 (void(*)) timer_ReadEClock,
 (void(*)) timer_SubTime,

 (APTR) ((UINT32)-1)
};

void lib_hexstrings ( unsigned int d );
#define STC_FREQ_HZ  1000000
#define TICK  100

struct TimerBase *timer_InitDev(struct TimerBase *TimerBase, UINT32 *segList, struct SysBase *SysBase)
{
	TimerBase->Device.dd_Library.lib_OpenCnt = 0;
	TimerBase->Device.dd_Library.lib_Node.ln_Pri = 0;
	TimerBase->Device.dd_Library.lib_Node.ln_Type = NT_DEVICE;
	TimerBase->Device.dd_Library.lib_Node.ln_Name = (STRPTR)DevName;
	TimerBase->Device.dd_Library.lib_Version = VERSION;
	TimerBase->Device.dd_Library.lib_Revision = REVISION;
	TimerBase->Device.dd_Library.lib_IDString = (STRPTR)Version;

	TimerBase->Timer_SysBase = SysBase;
	
    /* Setup the timer.device data */
	TimerBase->CurrentTime.tv_secs  = 0;
	TimerBase->CurrentTime.tv_micro = 0;

    TimerBase->VBlankTime.tv_secs = 0;
    TimerBase->VBlankTime.tv_micro = STC_FREQ_HZ / TICK; //TimerPeriod;
    TimerBase->Elapsed.tv_secs = 0;
    TimerBase->Elapsed.tv_micro = 0;

	NewList((struct List *) &TimerBase->Lists[UNIT_MICROHZ] );
	NewList((struct List *) &TimerBase->Lists[UNIT_VBLANK] );
	NewList((struct List *) &TimerBase->Lists[UNIT_ECLOCK] );
	NewList((struct List *) &TimerBase->Lists[UNIT_WAITUNTIL] );
	NewList((struct List *) &TimerBase->Lists[UNIT_WAITECLOCK] );

	//DPrintF("[Timer] TimerBase: %x, OpenDevice: %x\n", TimerBase, timer_BeginIO);
	
	TimerBase->TimerIntServer = CreateIntServer(DevName, TIMER_INT_PRI, TimerIRQServer, TimerBase);
	AddIntServer(IRQ_TIMER3, TimerBase->TimerIntServer);
	
	return TimerBase;
}


// ROMTAG Resident
struct InitTable
{
	UINT32	LibBaseSize;
	APTR	FunctionTable;
	APTR	DataTable;
	APTR	InitFunction;
} timer_InitTab = 
{
	sizeof(struct TimerBase), 
	timer_FuncTab,
	NULL,
	timer_InitDev
};
APTR TimerEndResident;

struct Resident TimerRomTag =
{
	RTC_MATCHWORD,
	&TimerRomTag,
	&TimerEndResident,
	RTF_AUTOINIT|RTF_COLDSTART,
	VERSION,
	NT_DEVICE,
	50,
	DevName,
	Version,
	0,
	&timer_InitTab
};

