#include "types.h"

#define STATUS_REG_IRQ_DISABLED_BIT (1 << 7)
#define STATUS_REG_MODE_MASK        0x1f
#define CP15_R1_HIGH_VECTORS_BIT    (1 << 13)
/* ARM Processor Operation Modes */
#define USER_MODE         0x10
#define FIQ_MODE          0x11
#define	IRQ_MODE          0x12
#define	SUPERVISOR_MODE   0x13
#define	ABORT_MODE        0x17
#define	UNDEFINED_MODE    0x1b
#define	SYSTEM_MODE       0x1f

/* [CS]PRS manipulation macros */
#define GEN_STATUS_READ(nm,reg) \
static inline UINT32 nm## _status_reg_read(void) \
{ \
	UINT32 retval; \
	asm volatile( \
		"mrs %[retval], " #reg \
		: [retval] "=r" (retval) \
	); \
	return retval; \
}

#define GEN_STATUS_WRITE(nm,reg,fieldname, field) \
static inline void nm## _status_reg_ ##fieldname## _write(UINT32 value) \
{ \
	asm volatile( \
		"msr " #reg "_" #field ", %[value]" \
		:: [value] "r" (value) \
	); \
}


/** Returns the value of CPSR (Current Program Status Register). */
GEN_STATUS_READ(current, cpsr)
/** Sets control bits of CPSR. */
GEN_STATUS_WRITE(current, cpsr, control, c);
/** Returns the value of SPSR (Saved Program Status Register). */
GEN_STATUS_READ(saved, spsr)


/** Disable interrupts.
 *
 * @return Old interrupt priority level.
 */
UINT32 interrupts_disable(void)
{
	UINT32 ipl = current_status_reg_read();
	current_status_reg_control_write(STATUS_REG_IRQ_DISABLED_BIT | ipl);
	return ipl;
}

/** Enable interrupts.
 *
 * @return Old interrupt priority level.
 */
UINT32 interrupts_enable(void)
{
	UINT32 ipl = current_status_reg_read();

	current_status_reg_control_write(ipl & ~STATUS_REG_IRQ_DISABLED_BIT);
	
	return ipl;
}

/** Restore interrupt priority level.
 *
 * @param ipl Saved interrupt priority level.
 */
void interrupts_restore(UINT32 ipl)
{
	current_status_reg_control_write(
	    (current_status_reg_read() & ~STATUS_REG_IRQ_DISABLED_BIT) | 
	    (ipl & STATUS_REG_IRQ_DISABLED_BIT));
}

/** Read interrupt priority level.
 *
 * @return Current interrupt priority level.
 */
UINT32 interrupts_read(void)
{
	return current_status_reg_read();
}

/** Check interrupts state.
 *
 * @return True if interrupts are disabled.
 *
 */
BOOL interrupts_disabled(void)
{
	return current_status_reg_read() & STATUS_REG_IRQ_DISABLED_BIT;
}

