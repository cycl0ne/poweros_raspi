#ifndef semaphores_h
#define semaphores_h

#include "types.h"
#include "list.h"
#include "ports.h"
#include "tasks.h"

struct SemaphoreMessage
{
    struct Message           ssm_Message;
    struct SignalSemaphore * ssm_Semaphore;
};

typedef struct SemaphoreRequest
{
	struct	MinNode	sr_Link;
	struct	Task	*sr_Waiter;
} SemaphoreRequest;

typedef struct SignalSemaphore
{
	struct	Node	ss_Link;
	struct	Task	*ss_Owner;
	UINT32			ss_NestCount;
	UINT32			ss_QueueCount;
	UINT32			ss_Type;
	struct	MinList	ss_WaitQueue;
	struct	SemaphoreRequest	ss_MultipleLink;
} SignalSemaphore;

#define SS_TYPE_SIMPLE	1
#define SS_TYPE_SHARED	2

#define SM_EXCLUSIVE (0L)
#define SM_SHARED    (1L)



#endif
