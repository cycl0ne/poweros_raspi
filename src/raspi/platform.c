#include "types.h"
#include "sysbase.h"

#include "memory.h"

#include "exec_funcs.h"
#include "raspi_platform.h"
#include "raspi_irq.h"

#include "context.h"

struct SysBase *g_SysBase;

void lib_ROMWackInit();
void lib_hexstrings ( unsigned int d );
void lib_Print_uart0(const char *s);

void lib_Permit(struct SysBase *SysBase);
void lib_Forbid(struct SysBase *SysBase);

void Platform_InitTimer();

void lib_Schedule(SysBase *SysBase);


#define NOP { __asm__ __volatile__ ("\tnop\n"); }
void debug_int(void)
{
	lib_hexstrings(READ32(ARM_IRQ_PEND1));
	lib_hexstrings(READ32(ARM_IRQ_PEND2));
	lib_hexstrings(READ32(ARM_IRQ_PEND3));
}

void USB_ShowTree();
void USB_Inf();
int usb_init(void);

#include "timer.h"

void sys_demo() 
{
	lib_Print_uart0("[DEMO] Started\n");	
	//	debug_int();
	
	SysBase *SysBase = g_SysBase;
	struct MsgPort *TimerPort = CreateMsgPort();
	struct TimeRequest *TimerIO = CreateIORequest(TimerPort, sizeof(struct TimeRequest));

//	lib_hexstrings(0xdeadc0de);
//	lib_hexstrings(SysBase->TDNestCnt);
//	lib_hexstrings(SysBase->IDNestCnt);
	
	UINT32 sel = READ32(GPFSEL1);
	sel &= ~(0b111 << 18);
	sel |= (0b001 << 18);
	WRITE32(GPFSEL1,sel);

	struct TimeRequest time;
	if (0 != OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)TimerIO, 0))
	{
		DPrintF("Open failed \n");
		for(;;);
	}
	
	TimerIO->tr_node.io_Command = TR_ADDREQUEST;
	TimerIO->tr_time.tv_secs = 5;
	TimerIO->tr_time.tv_micro = 0;
	
	//	Forbid();
	//	Disable();
//	usb_init();
//	Permit();
//	USB_ShowTree();
//	USB_Inf();
//	Enable();
	while(1) 
	{
		WRITE32(GPCLR0, 1<<16);
		TimerIO->tr_node.io_Command = TR_ADDREQUEST;
		TimerIO->tr_time.tv_secs = 5;
		TimerIO->tr_time.tv_micro = 0;
		DoIO((struct IORequest *)TimerIO);
		DPrintF("5Sec Timeout\n");
		//		for (int i = 0; i < 0x100000; i++) NOP;
		WRITE32(GPSET0, 1<<16);
		TimerIO->tr_node.io_Command = TR_ADDREQUEST;
		TimerIO->tr_time.tv_secs = 5;
		TimerIO->tr_time.tv_micro = 0;
		DoIO((struct IORequest *)TimerIO);		
		DPrintF("5Sec Timeout\n");
//		for (int i = 0; i < 0x100000; i++) NOP;
//		debug_int();

	}
}


struct MemHeader * INTERN_AddMemList(UINT32 size, UINT32 attribute, INT32 pri, APTR base, STRPTR name)
{
	struct MemHeader *mem;

	mem = (struct MemHeader *)base;
	mem->mh_Node.ln_Pri  = pri;
	mem->mh_Node.ln_Name = name;
	mem->mh_Node.ln_Type = NT_MEMORY;
	mem->mh_Attr = attribute;  
	mem->mh_First = NULL;

	mem->mh_Start = (struct MemChunk *)((UINT8 *)mem+(sizeof(struct MemHeader)));
	mem->mh_First = mem->mh_Start+1;
	mem->mh_Start->mc_Bytes = 0;
	mem->mh_Start->mc_Next = mem->mh_First;
	mem->mh_First->mc_Next = mem->mh_Start;
	mem->mh_First->mc_Bytes = ((((UINT8 *)base+size)) - ((UINT8*)base) -sizeof(MemChunk))/sizeof(MemChunk);
	/*  
	mem->mh_First = (struct MemChunk *)((UINT8 *)mem+(sizeof(struct MemHeader)));
	mem->mh_First->mc_Next = NULL;
	mem->mh_First->mc_Bytes = size-(sizeof(struct MemHeader));
	*/
	mem->mh_Lower = mem->mh_First;
	mem->mh_Upper = (APTR)((UINT8 *) base+size);
	mem->mh_Free  = mem->mh_First->mc_Bytes;//size-(sizeof(struct MemHeader));
	return mem;
}

Task *TaskCreate(SysBase *SysBase, char *name, APTR codeStart, APTR data, UINT32 stackSize, INT8 pri);

void install_exception_handlers(void);
void exception_init(void);
void install_exception_handlers(void);
SysBase *CreateSysBase(struct MemHeader *memStart);

#include "atag.h"

void ATAG_Scanner(SysBase *SysBase)
{
	UINT32 tag_value;
	//unsigned char *atags;
	UINT32 *atags;
	struct tag_cmdline *cmdline;
	//unsigned char *mac_param;
	//unsigned char mac[MAC_LEN + 1];

	for (atags = (UINT32 *)0x100; atags < (UINT32 *)0x8000; atags +=1)
	{
		//DPrintF("[%x][%x]", atags, atags[0]);
		switch (atags[1])
		{
			case ATAG_CORE:
				DPrintF("[ATAG_CORE] flags: %x, pagesize: %x, rootdev: %x\n", atags[2], atags[3], atags[4]);
				atags += atags[0]-1;
				break;
			case ATAG_MEM:
				DPrintF("[ATAG_MEM] size: %x, start: %x\n", atags[2], atags[3]);
				atags += atags[0]-1;
				break;
			case ATAG_VIDEOTEXT:
				DPrintF("[ATAG_VIDEOTEXT] x,y,video: %x, mode,cols, ega: %x, lines, vga, points: %x\n", atags[2], atags[3], atags[4]);
				atags += atags[0]-1;
				break;
			case ATAG_RAMDISK:
				DPrintF("[ATAG_RAMDISK] flags: %x, size: %x, start: %x\n", atags[2], atags[3], atags[4]);
				atags += atags[0]-1;
				break;
			case ATAG_INITRD2:
				DPrintF("[ATAG_INITRD2] size: %x, start: %x\n", atags[3], atags[2]);
				atags += atags[0]-1;
				break;
			case ATAG_SERIAL:
				DPrintF("[ATAG_SERIAL#] low: %x, high: %x\n", atags[2], atags[3]);
				atags += atags[0]-1;
				break;
			case ATAG_REVISION:
				DPrintF("[ATAG_REVISION] rev: %x\n", atags[2]);
				atags += atags[0]-1;
				break;
			case ATAG_VIDEOLFB:
				DPrintF("[ATAG_VIDEOLFB] found\n");//, atags[1], atags[2], atags[3]);
				atags += atags[0]-1;
				break;
			case ATAG_CMDLINE:
				DPrintF("[ATAG_CMDLINE] found: \n");
				atags+=2;
				cmdline = (struct tag_cmdline *)atags;
				DPrintF("%s\n", cmdline->cmdline);
				atags += atags[0]-1;
				break;
			case ATAG_ACORN:
				DPrintF("[ATAG_ACORN] found\n");
				atags += atags[0]-1;
				break;
			case ATAG_MEMCLK:
				DPrintF("[ATAG_MEMCLK] memclck: %x\n", atags[2]);
				atags += atags[0]-1;
				break;
			case ATAG_NONE:
				DPrintF("[ATAG_NONE] List ends\n");
				atags += atags[0]-1;	
				return;
				break;
			default:	
				break;
		}
	}
}

void ExecInit(void) {
	lib_ROMWackInit();
	exception_init();

	lib_Print_uart0("PowerOS ARM (Raspberry Pi Port)\n");
	int i=-1;
	lib_Print_uart0("[INIT] Adding Memory\n");
	struct MemHeader *mh = INTERN_AddMemList(0x4000000, MEMF_FAST, 0, (APTR)0x100000, "RASPI_Memory");
	//AddMemList(0x4000000, MEMF_FAST, 0, (APTR)0x100000, "RASPI_Memory");

	lib_Print_uart0("[INIT] SysBase Preperation\n");
	g_SysBase = CreateSysBase(mh);
	SysBase *SysBase = g_SysBase;
	
	DPrintF("[INIT] ATAG Scanner\n");
	ATAG_Scanner(g_SysBase);
	
	Platform_InitTimer();

	DPrintF("[INIT] Activating SysBase Permit/Enable -> Leaving SingleTask\n");
	
	SysBase->TDNestCnt = -1;
	SysBase->IDNestCnt = -1;

	Task *idle2 = TaskCreate(SysBase, "demo", sys_demo, SysBase, 4096*2 , 0);

	DPrintF("[INIT] Schedule -> leaving Kernel Init\n");
	Schedule();
	DPrintF("[INIT] PONR (point of no return\n");
}

StackContext Exec_Context;
UINT8 Exec_Stack[4096*4];
//UINT8 *Exec_Stack = (UINT8*)&ExecStack[2];

__attribute__((no_instrument_function))  void main_bsp(void) {
	context_save(&Exec_Context);
	context_set(&Exec_Context, FADDR(ExecInit), Exec_Stack, 4096*4);
	context_restore(&Exec_Context);
}
