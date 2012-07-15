#include "types.h"
#include "sysbase.h"

#include "raspi_platform.h"
#include "raspi_irq.h"

#include "arch_irq.h"
#include "exec_funcs.h"

extern SysBase *g_SysBase;

syshandler_t syscall_table[1] = {
(syshandler_t)NULL
};

exc_table_t exc_table[7];

void install_exception_handlers(void);
extern void exception_init(void);
extern void reset_exception_entry(void);
extern void irq_exception_entry(void);
extern void fiq_exception_entry(void);
extern void undef_instr_exception_entry(void);
extern void prefetch_abort_exception_entry(void);
extern void data_abort_exception_entry(void);
extern void swi_exception_entry(void);
extern void prefetch_abort(unsigned int, istate *);
extern void data_abort(unsigned int, istate *);

extern APTR supervisor_sp;


void lib_hexstrings ( unsigned int d );
void lib_Print_uart0(const char *s);

void arch_pre_main(void *entry __attribute__((unused)), APTR bootinfo)
{
}

/** Handles "data abort" exception (load or store at invalid address).
 *
 * @param exc_no Exception number.
 * @param istate CPU state when exception occured.
 *
 */
void data_abort(unsigned int exc_no, istate *istate)
{
	lib_Print_uart0("DATA ABORT");
	for(;;);
}

/** Handles "prefetch abort" exception (instruction couldn't be executed).
 *
 * @param exc_no Exception number.
 * @param istate CPU state when exception occured.
 *
 */
void prefetch_abort(unsigned int exc_no, istate *istate)
{
	lib_Print_uart0("PREFETCH ABORT");
	for(;;);
}

sysarg_t syscall_handler(sysarg_t a1, sysarg_t a2, sysarg_t a3, sysarg_t a4, sysarg_t a5, sysarg_t a6, sysarg_t id)
{
	sysarg_t rc;
	if (id < 0) {
		rc = syscall_table[id](a1, a2, a3, a4, a5, a6);
	} else {
		//printf("Task %" PRIu64": Unknown syscall %#" PRIxn, TASK->taskid, id);
		//task_kill_self(true);
	}
		
	return rc;
}

static void install_handler(unsigned handler_addr, unsigned *vector)
{
	/* relative address (related to exc. vector) of the word
	 * where handler's address is stored
	*/
	volatile UINT32 handler_address_ptr = EXC_VECTORS_SIZE - PREFETCH_OFFSET;
	
	/* make it LDR instruction and store at exception vector */
	*vector = handler_address_ptr | LDR_OPCODE;
	//FIXME: smc_coherence(*vector);
	
	/* store handler's address */
	*(vector + EXC_VECTORS) = handler_addr;
}

void swi_exception(unsigned int exc_no, istate *istate)
{
	istate->r0 = syscall_handler(istate->r0, istate->r1, istate->r2,
	    istate->r3, istate->r4, istate->r5, istate->r6);
}

/** Fills exception vectors with appropriate exception handlers. */
void install_exception_handlers(void)
{
	install_handler((unsigned) reset_exception_entry,
	    (unsigned *) EXC_RESET_VEC);
	
	install_handler((unsigned) undef_instr_exception_entry,
	    (unsigned *) EXC_UNDEF_INSTR_VEC);
	
	install_handler((unsigned) swi_exception_entry,
	    (unsigned *) EXC_SWI_VEC);
	
	install_handler((unsigned) prefetch_abort_exception_entry,
	    (unsigned *) EXC_PREFETCH_ABORT_VEC);
	
	install_handler((unsigned) data_abort_exception_entry,
	    (unsigned *) EXC_DATA_ABORT_VEC);
	
	install_handler((unsigned) irq_exception_entry,
	    (unsigned *) EXC_IRQ_VEC);
	
	install_handler((unsigned) fiq_exception_entry,
	    (unsigned *) EXC_FIQ_VEC);
}

/** Default 'null' exception handler
 *
 */
__attribute__((no_instrument_function)) void exc_undef(unsigned int exc_no, istate *istate, APTR Data, SysBase *SysBase)
{
	DPrintF("Unhandled exception #%d", exc_no);
	for(;;);
}


void exception_init(void)
{
	install_exception_handlers();
}
