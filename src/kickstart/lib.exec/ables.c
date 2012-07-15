#include "types.h"
#include "sysbase.h"
void lib_hexstrings ( unsigned int d );
void lib_Print_uart0(const char *s);

#include "raspi_irq.h"
#include "raspi_platform.h"
UINT32 interrupts_disable(void);
UINT32 interrupts_enable(void);

static void arch_Enable(void)
{
	interrupts_enable();
}

static void arch_Disable(void)
{
	interrupts_disable();
}


void lib_Enable(struct SysBase *SysBase)
{
  	SysBase->IDNestCnt--;
//	lib_Print_uart0("Enable\n");
	if (SysBase->IDNestCnt < 0) arch_Enable();
}

void lib_Disable(struct SysBase *SysBase)
{
	arch_Disable();
	SysBase->IDNestCnt++;
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

