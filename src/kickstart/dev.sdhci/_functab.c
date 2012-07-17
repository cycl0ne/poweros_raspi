#include "sdhci_intern.h"

#undef SysBase
#include "sysbase.h"
#include "resident.h"

// Include our Protos
#include "exec_funcs.h"

static char DevName[] = "sdhci.device";
static char Version[] = "\0$VER: sdhci.device 0.1 (__DATE__)\r\n";
#define SDHCI_INT_PRI 0
#define SDHCI_INT_NR  INTERRUPT_DMA4

APTR sdhci_ExpungeDev(struct SDHCIBase *SDHCIBase);
APTR sdhci_ExtFuncDev(struct SDHCIBase *SDHCIBase);
APTR sdhci_OpenDev(struct SDHCIBase *SDHCIBase, struct IORequest *ioreq, UINT32 unitnum, UINT32 flags);
APTR sdhci_CloseDev(struct SDHCIBase *SDHCIBase, struct IORequest *ioreq);
void sdhci_BeginIO(SDHCIBase *SDHCIBase, struct IORequest *ioreq);
void sdhci_AbortIO(SDHCIBase *SDHCIBase, struct IORequest *ioreq);
__attribute__((no_instrument_function)) BOOL SDHCI_IRQServer(UINT32 number, istate* istate, SDHCIBase *SDHCIBase, APTR SysBase);


APTR sdhci_FuncTab[] = 
{
	(void(*)) sdhci_OpenDev,
	(void(*)) sdhci_CloseDev,
	(void(*)) sdhci_ExpungeDev,
	(void(*)) sdhci_ExtFuncDev,

	(void(*)) sdhci_BeginIO,
	(void(*)) sdhci_AbortIO,
	(APTR) ((UINT32)-1)
};

#define _SDHCITASK_STACK_ 0x8000
void dev_SDHCITask(struct SDHCIBase *SDHCIBase);
int mmc_initialize(APTR);

struct SDHCIBase *sdhci_InitDev(struct SDHCIBase *SDHCIBase, UINT32 *segList, struct SysBase *SysBase)
{
	DPrintF("Init SDHCI %x\n", sizeof(struct SDHCIBase));
	SDHCIBase->Device.dd_Library.lib_OpenCnt = 0;
	SDHCIBase->Device.dd_Library.lib_Node.ln_Pri = 0;
	SDHCIBase->Device.dd_Library.lib_Node.ln_Type = NT_DEVICE;
	SDHCIBase->Device.dd_Library.lib_Node.ln_Name = (STRPTR)DevName;
	SDHCIBase->Device.dd_Library.lib_Version = VERSION;
	SDHCIBase->Device.dd_Library.lib_Revision = REVISION;
	SDHCIBase->Device.dd_Library.lib_IDString = (STRPTR)Version;

	SDHCIBase->SysBase = SysBase;
	// m/u/ndelay Timer
	//if (0 != OpenDevice("timer.device", UNIT_MICROHZ, (struct IORequest *)&SDHCIBase->Timer0_Unit, 0)) return NULL;

	
//	SDHCIBase->SDHCI_IntServer = CreateIntServer(DevName, SDHCI_INT_PRI, SDHCI_IRQServer, SDHCIBase);
//	AddIntServer(SDHCI_INT_NR, SDHCIBase->SDHCI_IntServer);

	SDHCIBase->SDHCITask = TaskCreate(DevName, dev_SDHCITask, SDHCIBase, _SDHCITASK_STACK_, 10);

	#define BCM2835_ARM_MBOX0_BASE 0x2000b880
	#define BCM2835_ARM_MBOX1_BASE 0x2000b8a0
	#define MBOX_CHAN_POWER   0 /* for use by the power management interface */
	#define MBOX_MSG(chan, data28)      (((data28) & ~0xf) | ((chan) & 0xf))

	volatile UINT32 *mbox0_read = (volatile UINT32 *)BCM2835_ARM_MBOX0_BASE;
	volatile UINT32 *mbox0_write = (volatile UINT32 *)BCM2835_ARM_MBOX1_BASE;
	volatile UINT32 *mbox0_status = (volatile UINT32 *)(BCM2835_ARM_MBOX0_BASE + 0x18);
	volatile UINT32 *mbox0_config = (volatile UINT32 *)(BCM2835_ARM_MBOX0_BASE + 0x1C);
	UINT32 val = 0;
	// SDHC Poweron
	val |= 1 << 4;

	while (*mbox0_status & 0x80000000) {}
	*mbox0_write = MBOX_MSG(0, val); 

	mmc_initialize(SysBase);

	return SDHCIBase;
}

// ROMTAG Resident
struct InitTable
{
	UINT32	LibBaseSize;
	APTR	FunctionTable;
	APTR	DataTable;
	APTR	InitFunction;
} static sdhci_InitTab = 
{
	sizeof(struct SDHCIBase), 
	sdhci_FuncTab,
	NULL,
	sdhci_InitDev
};
APTR SDHCIEndResident;

static struct Resident SDHCIRomTag =
{
	RTC_MATCHWORD,
	&SDHCIRomTag,
	&SDHCIEndResident,
	RTF_AUTOINIT|RTF_COLDSTART,
	VERSION,
	NT_DEVICE,
	30,
	DevName,
	Version,
	0,
	&sdhci_InitTab
};


