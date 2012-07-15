#include "types.h"
#include "sysbase.h"
#include "raspi_platform.h"
#include "raspi_irq.h"
#include "exec_funcs.h"

extern SysBase *g_SysBase;

#define STC_FREQ_HZ  1000000
#define TIMER_PERIOD 10000	
#define NSEC_PER_SEC    1000000000L
#define SHIFT 32


extern struct FuncTab funcTab;
void lib_hexstrings ( unsigned int d );
void lib_Print_uart0(const char *s);
	
static void IRQ_Empty(APTR SysBase, UINT32 num)
{
	lib_Print_uart0("IRQ_Empty\n");
	lib_hexstrings(0xbadc0de);
	lib_hexstrings(num);
}

__attribute__((no_instrument_function))  UINT32 IRQ_Tick(unsigned int exc_no, istate *istate, APTR Data, SysBase *SysBase)
{
//	lib_hexstrings(0x00dec0de);
	WRITE32(ST_BASE + 0x00, 1<<3);
	UINT32 stc = READ32(ST_BASE + 0x04);
	WRITE32(ST_BASE + 0x18, stc + STC_FREQ_HZ/100);//20); // We are running here with 1microsecond per inc. 50 taskswitch per second

	Schedule();
	return 1;
}

void Platform_InitTimer()
{
	lib_Print_uart0("[INIT] Timer Activation\n");
	SysBase *SysBase = g_SysBase;
	struct Interrupt *irq;
	
	irq = CreateIntServer("Tick (exec.library)", -10, IRQ_Tick, NULL);
	AddIntServer(IRQ_TIMER3, irq);
	
//	for (int i = 0; i<NR_IRQS; i++) SysBase->IRQHandler[i] = &IRQ_Empty;
	WRITE32(ARM_T_CONTROL, 0x00000000);
//	SysBase->IRQHandler[IRQ_TIMER3] = &IRQ_Tick;
	UINT32 stc = READ32(ST_BASE + 0x04);
	WRITE32(ST_BASE+0x18, stc + STC_FREQ_HZ/100); //TIMER_PERIOD);
	WRITE32(ARM_T_CONTROL, 1<<5);
	WRITE32(ARM_IRQ_ENBL1, 1<<3);
	WRITE32(ARM_IRQ_PEND1, 0x00000000);
}

// The STC is a free running counter that increments at the rate of 1MHz
UINT32 readTicks()
{
	return READ32(ST_BASE+0x04);
}
