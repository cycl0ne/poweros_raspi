#include "usbbase_internal.h"
#undef SysBase

#include "sysbase.h"
#include "resident.h"

// Include our Protos
#include "exec_funcs.h"


static char LibName[] = "usb.library";
static char Version[] = "\0$VER: usb.library 0.1 (__DATE__)\r\n";
#define USB_INT_PRI 0

//__attribute__((no_instrument_function)) BOOL TimerIRQServer(UINT32 number, istate* istate, TimerBase *TimerBase, APTR SysBase);


APTR usb_FuncTab[] = 
{
// (void(*)) timer_OpenDev,
// (void(*)) timer_CloseDev,
// (void(*)) timer_ExpungeDev,
// (void(*)) timer_ExtFuncDev,

 (APTR) ((UINT32)-1)
};

void lib_hexstrings ( unsigned int d );
#define UsbBase(a) UsbBase->a
extern struct UsbBase *UsbBase_;
int usb_init(void);


#define BCM2835_ARM_MBOX0_BASE 0x2000b880
#define BCM2835_ARM_MBOX1_BASE 0x2000b8a0
#define MBOX_CHAN_POWER   0 /* for use by the power management interface */
#define MBOX_MSG(chan, data28)      (((data28) & ~0xf) | ((chan) & 0xf))


struct UsbBase *usb_InitLib(struct UsbBase *UsbBase, UINT32 *segList, struct SysBase *SysBase)
{
   	UsbBase->LibNode.lib_Node.ln_Type = NT_LIBRARY;
   	UsbBase->LibNode.lib_Version      = VERSION;
   	UsbBase->LibNode.lib_Revision     = REVISION;
   	UsbBase->LibNode.lib_Node.ln_Name = (STRPTR)LibName;
   	UsbBase->LibNode.lib_IDString     = (STRPTR)Version;
   	UsbBase->LibNode.lib_OpenCnt      = 0;
	UsbBase->LibNode.lib_Node.ln_Pri  = 0;
	UsbBase->USB_SysBase = SysBase;

	// Ugly for Wrapper
	UsbBase_ = UsbBase;
	//TimerBase->TimerIntServer = CreateIntServer(DevName, TIMER_INT_PRI, TimerIRQServer, TimerBase);
	//AddIntServer(IRQ_TIMER3, TimerBase->TimerIntServer);
	
	UsbBase->Running = 0;
	UsbBase->Dev_Index = 0;
	UsbBase->Asynch_Allowed = 1;
	/*
	 * Request power for USB 
	 */
	volatile UINT32 *mbox0_read = (volatile UINT32 *)BCM2835_ARM_MBOX0_BASE;
	volatile UINT32 *mbox0_write = (volatile UINT32 *)BCM2835_ARM_MBOX1_BASE;
	volatile UINT32 *mbox0_status = (volatile UINT32 *)(BCM2835_ARM_MBOX0_BASE + 0x18);
	volatile UINT32 *mbox0_config = (volatile UINT32 *)(BCM2835_ARM_MBOX0_BASE + 0x1C);
	UINT32 val = 0;

	val |= 8 << 4;
	val |= 1 << 4;

	while (*mbox0_status & 0x80000000) {
	}

	*mbox0_write = MBOX_MSG(0, val); 
	//usb_init();
	return UsbBase;
}


// ROMTAG Resident
static struct InitTable
{
	UINT32	LibBaseSize;
	APTR	FunctionTable;
	APTR	DataTable;
	APTR	InitFunction;
} usb_InitTab = 
{
	sizeof(struct UsbBase), 
	usb_FuncTab,
	NULL,
	usb_InitLib
};
static APTR UsbEndResident;

struct Resident UsbRomTag =
{
	RTC_MATCHWORD,
	&UsbRomTag,
	&UsbEndResident,
	RTF_AUTOINIT|RTF_COLDSTART,
	VERSION,
	NT_LIBRARY,
	50,
	LibName,
	Version,
	0,
	&usb_InitTab
};
