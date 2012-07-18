#include "types.h"
#include "sysbase.h"
void lib_hexstrings ( unsigned int d );
void lib_Print_uart0(const char *s);

#include "raspi_irq.h"
#include "raspi_platform.h"
UINT32 interrupts_disable(void);
UINT32 interrupts_enable(void);
void interrupts_restore(UINT32 ipl);

static void arch_Enable(void)
{
	interrupts_enable();
}

static UINT32 arch_Disable(void)
{
	return interrupts_disable();
}

void lib_Enable(struct SysBase *SysBase, UINT32 ipl)
{
//  	SysBase->IDNestCnt--;
//	lib_Print_uart0("Enable\n");
//	if (SysBase->IDNestCnt < 0) arch_Enable();
	interrupts_restore(ipl);
}

UINT32 lib_Disable(struct SysBase *SysBase)
{
	return interrupts_disable();
	//if (SysBase->IDNestCnt < 0) return interrupts_disable();
	//SysBase->IDNestCnt++;
}

void lib_Permit(struct SysBase *SysBase)
{
//	lib_Print_uart0("Permit\n");
  	SysBase->TDNestCnt--;
//	lib_Print_uart0("Permit\n");
  	//if (SysBase->IDNestCnt < 0) arch_Enable();
}

void lib_Forbid(struct SysBase *SysBase)
{
//	lib_Print_uart0("Forbid\n");
  	SysBase->TDNestCnt++;
//	lib_Print_uart0("Forbid\n");
  	//if (SysBase->IDNestCnt < 0) arch_Enable();
}

