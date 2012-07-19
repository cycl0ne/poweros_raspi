#ifndef tasks_h
#define tasks_h

#include "types.h"
#include "list.h"
typedef enum Task_State 
{
	INVALID,
	ADDED,
	RUN,
	READY,
	WAIT,
	EXCEPT,
	REMOVED,
	DEAD
} Task_State;

typedef struct {
	UINT32 cpu_mode;
	UINTPTR sp;
	UINTPTR pc;
	
	UINT32 r4;
	UINT32 r5;
	UINT32 r6;
	UINT32 r7;
	UINT32 r8;
	UINT32 r9;
	UINT32 r10;
	UINT32 fp;	/* r11 */
	
	UINT32 ipl;
} StackContext;

typedef void (*Task_Function)(void *data);

#define TF_PROCTIME	(1L<<0)
#define TF_ETASK	(1L<<3)
#define TF_STACKCHK	(1L<<4)
#define TF_EXCEPT	(1L<<5)
#define TF_SWITCH	(1L<<6)
#define TF_LAUNCH	(1L<<7)

// Special Signals SYSTEMUSE
#define SIGB_ABORT	0
#define SIGB_CHILD	1
#define SIGB_BLIT	4	/* Note: same as SINGLE */
#define SIGB_SINGLE	4	/* Note: same as BLIT */
#define SIGB_SEMAPHORE	4	/* Note: same as both */
#define SIGB_INTUITION	5
#define SIGB_NET	7
#define SIGB_DOS	8

#define SIGF_SYSTEM  SIGF_DOS||SIGF_NET||SIGF_INTUITION||SIGF_SINGLE  //Reserved for Systemuse

#define SIGF_ABORT	(1L<<0)
#define SIGF_CHILD	(1L<<1)

#define SIGF_BLIT	(1L<<4)
#define SIGF_SINGLE	(1L<<4)
#define SIGF_SEMAPHORE	(1L<<4)

#define SIGF_INTUITION	(1L<<5)
#define SIGF_NET	(1L<<7)
#define SIGF_DOS	(1L<<8)

typedef struct Task 
{
	Node				Node;
	UINT16				Flags;
	UINT8				IDNestCnt;
	UINT8				TDNestCnt;
	volatile Task_State	State;
	UINT32				CPU_Usage;
	Task_Function	Switch;
	Task_Function	Launch;
	UINT32				SigAlloc;
	UINT32				SigWait;
	UINT32				SigRecvd;
	UINT32				SigExcept;
	APTR				TaskFunc;
	APTR				TaskArg;
	StackContext		SavedContext;
	StackContext		WaitContext;
	UINT8				*Stack;
	UINT32				StackSize;
} Task;	
	
#endif
