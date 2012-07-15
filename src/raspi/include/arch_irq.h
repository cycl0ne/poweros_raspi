#ifndef arch_irq_h
#define arch_irq_h

#include "types.h"

#define PREFETCH_OFFSET      0x8
/** LDR instruction's code */
#define LDR_OPCODE           0xe59ff000
/** Number of exception vectors. */
#define EXC_VECTORS          8
/** Size of memory block occupied by exception vectors. */
#define EXC_VECTORS_SIZE     (EXC_VECTORS * 4)

#define EXC_BASE_ADDRESS  0x0
/* Exception Vectors */
#define EXC_RESET_VEC           (EXC_BASE_ADDRESS + 0x0)
#define EXC_UNDEF_INSTR_VEC     (EXC_BASE_ADDRESS + 0x4)
#define EXC_SWI_VEC             (EXC_BASE_ADDRESS + 0x8)
#define EXC_PREFETCH_ABORT_VEC  (EXC_BASE_ADDRESS + 0xc)
#define EXC_DATA_ABORT_VEC      (EXC_BASE_ADDRESS + 0x10)
#define EXC_IRQ_VEC             (EXC_BASE_ADDRESS + 0x18)
#define EXC_FIQ_VEC             (EXC_BASE_ADDRESS + 0x1c)

/* Exception numbers */
#define EXC_RESET           0
#define EXC_UNDEF_INSTR     1
#define EXC_SWI             2
#define EXC_PREFETCH_ABORT  3
#define EXC_DATA_ABORT      4
#define EXC_IRQ             5
#define EXC_FIQ             6


typedef struct istate {
	UINT32 spsr;
	UINT32 sp;
	UINT32 lr;
	
	UINT32 r0;
	UINT32 r1;
	UINT32 r2;
	UINT32 r3;
	UINT32 r4;
	UINT32 r5;
	UINT32 r6;
	UINT32 r7;
	UINT32 r8;
	UINT32 r9;
	UINT32 r10;
	UINT32 fp;
	UINT32 r12;
	
	UINT32 pc;
} istate;

typedef  BOOL (* iroutine)(unsigned int, istate *, APTR, APTR);

typedef UINT32 sysarg_t;

typedef sysarg_t (*syshandler_t)(sysarg_t, sysarg_t, sysarg_t, sysarg_t,
    sysarg_t, sysarg_t);

typedef struct {
	const char *name;
	iroutine handler;
	UINT32 cycles;
	UINT32 count;
} exc_table_t;


#endif
