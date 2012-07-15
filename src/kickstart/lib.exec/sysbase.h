#ifndef sysbase_h
#define sysbase_h

#include "types.h"
#include "library.h"
#include "tasks.h"

#include "raspi_irq.h"

typedef void(*IRQHandler)(APTR, UINT32);

typedef struct SysBase {
	Library		LibNode;
	Task		*thisTask;
	UINT32		IdleCnt;
	UINT32		DispCnt;
	UINT32		Quantum;
	UINT32		QUsed;
	UINT16		SysFlags;
	INT8		TDNestCnt;
	INT8		IDNestCnt;
	List		TaskReady;
	List		TaskWait;
	List		PortList;
	List		MemList;
	List		DevList;
	List		LibList;
	List		SemaphoreList;
	List		ResourceList;
	List		ResidentList;
	List		ExcServer[8];
	List		IRQServer[NR_IRQS];
	UINT8		*CPU_Stack;
	StackContext CPU_Context;
} SysBase;

#define _CPUSTACK_ 0x1000

#endif
