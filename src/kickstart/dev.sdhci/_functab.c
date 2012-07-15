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

struct SDHCIBase *sdhci_OpenDev(struct SDHCIBase *SDHCIBase, struct IORequest *ioreq, UINT32 unitNum, UINT32 flags);
APTR sdhci_CloseDev(struct SDHCIBase *SDHCIBase, struct IORequest *ioreq);
UINT32 *sdhci_ExpungeDev(struct SDHCIBase *SDHCIBase);
UINT32 sdhci_ExtFuncDev(void);
void sdhci_BeginIO(SDHCIBase *SDHCIBase, struct IORequest *ioreq);
void sdhci_AbortIO(SDHCIBase *SDHCIBase, struct IORequest *ioreq);
__attribute__((no_instrument_function)) BOOL SDHCI_IRQServer(UINT32 number, istate* istate, SDHCIBase *SDHCIBase, APTR SysBase);


APTR sdhci_FuncTab[] = 
{
// (void(*)) sdhci_OpenDev,
// (void(*)) sdhci_CloseDev,
// (void(*)) sdhci_ExpungeDev,
// (void(*)) sdhci_ExtFuncDev,

// (void(*)) sdhci_BeginIO,
// (void(*)) sdhci_AbortIO,

 (APTR) ((UINT32)-1)
};

struct SDHCIBase *sdhci_InitDev(struct SDHCIBase *SDHCIBase, UINT32 *segList, struct SysBase *SysBase)
{
	SDHCIBase->Device.dd_Library.lib_OpenCnt = 0;
	SDHCIBase->Device.dd_Library.lib_Node.ln_Pri = 0;
	SDHCIBase->Device.dd_Library.lib_Node.ln_Type = NT_DEVICE;
	SDHCIBase->Device.dd_Library.lib_Node.ln_Name = (STRPTR)DevName;
	SDHCIBase->Device.dd_Library.lib_Version = VERSION;
	SDHCIBase->Device.dd_Library.lib_Revision = REVISION;
	SDHCIBase->Device.dd_Library.lib_IDString = (STRPTR)Version;

	SDHCIBase->SysBase = SysBase;
	
//	if (0 != OpenDevice("timer.device", UNIT_MICROHZ, (struct IORequest *)&SDHCIBase->Timer0_Unit, 0)) return NULL;
//	if (0 != OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)&SDHCIBase->Timer1_Unit, 0)) return NULL;
	
	SDHCIBase->Timer0_Dev = SDHCIBase->Timer0_Unit.tr_node.io_Device;
	SDHCIBase->Timer1_Dev = SDHCIBase->Timer1_Unit.tr_node.io_Device;
	
//	SDHCIBase->SDHCI_IntServer = CreateIntServer(DevName, SDHCI_INT_PRI, SDHCI_IRQServer, SDHCIBase);
//	AddIntServer(SDHCI_INT_NR, SDHCIBase->SDHCI_IntServer);
	
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


