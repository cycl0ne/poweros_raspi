#include "types.h"
#include "arch_irq.h"
#include "list.h"
#include "irq.h"
#include "sysbase.h"
#include "exec_funcs.h"

#include "raspi_platform.h"
#include "raspi_irq.h"


void lib_hexstrings ( unsigned int d );
void lib_Print_uart0(const char *s);

extern SysBase *g_SysBase;

void lib_AddExcServer(SysBase *SysBase, UINT32 intNumber, struct Interrupt *isr)
{
	UINT32 ipl;
	if (NULL==isr) return;
	if (intNumber>EXC_VECTORS) return;
	isr->is_Cycles = 0;
	isr->is_Count = 0;
	
	ipl = Disable();
	Enqueue((struct List *)&SysBase->ExcServer[intNumber], (struct Node *)isr);
	Enable(ipl);
	if (!IsListEmpty(&SysBase->ExcServer[intNumber])) 
	{
	    //if (intNumber>=8) arch_unmask_irq(2);
	    //arch_unmask_irq(intNumber);
	    //DPrintF("exec - Unmasked Int %d\n", intNumber);
	}
}

void lib_AddIntServer(SysBase *SysBase, UINT32 intNumber, struct Interrupt *isr)
{
	UINT32 ipl;
	if (NULL==isr) return;
	if (intNumber < 0 && intNumber > NR_IRQS) return;
	isr->is_Cycles = 0;
	isr->is_Count = 0;
	
	ipl = Disable();
	Enqueue((struct List *)&SysBase->IRQServer[intNumber], (struct Node *)isr);
	Enable(ipl);
	if (!IsListEmpty(&SysBase->IRQServer[intNumber])) 
	{
	    //if (intNumber>=8) arch_unmask_irq(2);
	    //arch_unmask_irq(intNumber);
	    //DPrintF("exec - Unmasked Int %d\n", intNumber);
	}
}

struct Interrupt *lib_RemExcServer(SysBase *SysBase, UINT32 intNumber, struct Interrupt *isr)
{
	UINT32 ipl;
	if (NULL==isr) return NULL;
	if (intNumber < 0 && intNumber > EXC_VECTORS) return NULL;
	ipl = Disable();
	Remove((struct Node *)isr);
	Enable(ipl);
	//if (IsListEmpty(&SysBase->IntSrv[intNumber])) arch_mask_irq(intNumber);
	return NULL;
}

struct Interrupt *lib_RemIntServer(SysBase *SysBase, UINT32 intNumber, struct Interrupt *isr)
{
	UINT32 ipl;
	if (NULL==isr) return NULL;
	if (intNumber < 0 && intNumber > NR_IRQS) return NULL;
	ipl = Disable();
	Remove((struct Node *)isr);
	Enable(ipl);
	//if (IsListEmpty(&SysBase->IntSrv[intNumber])) arch_mask_irq(intNumber);
	return NULL;
}

struct Interrupt *lib_CreateIntServer(SysBase *SysBase, const STRPTR name, INT8 pri, APTR handler, APTR data)
{
	struct Interrupt *irq;
	irq = AllocVec(sizeof(struct Interrupt), MEMF_FAST);	
	if (irq) {
	
		irq->is_Node.ln_Name = name;
		irq->is_Node.ln_Pri = pri;
		irq->is_Node.ln_Type = NT_INTERRUPT;
		irq->is_Handler = (iroutine) handler;
		irq->is_Data = data;
		return irq; 
	}
	return NULL;
}

extern __attribute__((no_instrument_function)) void exc_undef(unsigned int exc_no, istate *istate, APTR Data, SysBase *SysBase);
extern  __attribute__((no_instrument_function)) void irq_exception(unsigned int exc_no, istate *istate);
extern void swi_exception(unsigned int exc_no, istate *istate);
extern void prefetch_abort(unsigned int exc_no, istate *istate);
extern void data_abort(unsigned int exc_no, istate *istate);
__attribute__((no_instrument_function))  void _IRQServer(unsigned int exc_no, istate *istate, APTR Data, SysBase *SysBase);

void _InitExcServer(SysBase *SysBase)
{
	struct Interrupt *irq;
	irq = CreateIntServer("Raspi IRQ Handler", -10, _IRQServer, NULL);
	AddExcServer(EXC_IRQ, irq);
	irq = CreateIntServer("Raspi prefetch abort", -10, prefetch_abort, NULL);
	AddExcServer(EXC_PREFETCH_ABORT, irq);
	irq = CreateIntServer("Raspi data abort", -10, data_abort, NULL);
	AddExcServer(EXC_DATA_ABORT, irq);
	irq = CreateIntServer("Raspi software irq", -10, swi_exception, NULL);
	AddExcServer(EXC_SWI, irq);

	irq = CreateIntServer("Unhandled Exception", -10, exc_undef, NULL);
	AddExcServer(EXC_FIQ_VEC, irq);
	irq = CreateIntServer("Unhandled Exception", -10, exc_undef, NULL);
	AddExcServer(EXC_UNDEF_INSTR, irq);
	irq = CreateIntServer("Unhandled Exception", -10, exc_undef, NULL);
	AddExcServer(EXC_RESET, irq);	
}

__attribute__((no_instrument_function)) void _ExcServer(UINT32 number, istate *istate)
{
	struct Interrupt *irq;
	SysBase *SysBase = g_SysBase;
	
	ForeachNode(&SysBase->ExcServer[number], irq)
	{
		if (irq->is_Handler(number, istate, irq->is_Data, g_SysBase)) 
		{
			irq->is_Count++;
			break;
		}
	}
}

static const int MultiplyDeBruijnBitPosition2[32] = 
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};

__attribute__((no_instrument_function))  void _IRQServer(unsigned int exc_no, istate *istate, APTR Data, SysBase *SysBase)
{
	struct Interrupt *irq;
	int i;
	//DPrintF("int: %d", exc_no);
	UINT32	IRQBits = READ32(ARM_IRQ_PEND3);

	if (CHECK_BIT(IRQBits, 8)) 
	{
		IRQBits = READ32(ARM_IRQ_PEND1);
		UINT32 number = MultiplyDeBruijnBitPosition2[(UINT32)(IRQBits * 0x077CB531U) >> 27];

		ForeachNode(&SysBase->IRQServer[number], irq)
		{
			if (irq->is_Handler(number, istate, irq->is_Data, SysBase)) 
			{
				irq->is_Count++;
				break;
			}
		}
		WRITE32(ARM_IRQ_PEND3, 0x00000000); // Not sure?!
		WRITE32(ARM_IRQ_PEND1, 0x00000000); // Not sure?!
		return;
	}

	if (CHECK_BIT(IRQBits, 9)) 
	{
		IRQBits = READ32(ARM_IRQ_PEND2);
		UINT32 number = MultiplyDeBruijnBitPosition2[(UINT32)(IRQBits * 0x077CB531U) >> 27];

		i = number + 32;
		ForeachNode(&SysBase->IRQServer[i], irq)
		{
			if (irq->is_Handler(i, istate, irq->is_Data, SysBase)) 
			{
				irq->is_Count++;
				break;
			}
		}
		WRITE32(ARM_IRQ_PEND3, 0x00000000); // Not sure?!
		WRITE32(ARM_IRQ_PEND2, 0x00000000); // Not sure?!
		return;
	}

	/* Perhaps we could improve this by only checking Active IRQs*/
	IRQBits = READ32(ARM_IRQ_PEND3);
	UINT32 number = MultiplyDeBruijnBitPosition2[(UINT32)(IRQBits * 0x077CB531U) >> 27];
	i = number + 64;
	ForeachNode(&SysBase->IRQServer[i], irq)
	{
		if (irq->is_Handler(i, istate, irq->is_Data, SysBase)) 
		{
			irq->is_Count++;
			break;
		}
	}
	WRITE32(ARM_IRQ_PEND3, 0x00000000); // Not sure?!
	return;
}


