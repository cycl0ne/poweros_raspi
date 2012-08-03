#ifndef ExpansionBase_h
#define ExpansionBase_h

#include "types.h"
#include "library.h"
#include "semaphores.h"

#include "dos.h"

struct ExpansionBase
{
	struct Library			Library;
	APTR					SysBase;
	APTR					DosBase;
	UINT32					Flags;
	struct SignalSemaphore	BoardListLock;
	List					BoardList; // Holds the PCI/PCIe Board list (not used on Raspi)
	struct SignalSemaphore	MountListLock;
	List					MountList;
};

struct ExpDosNode
{
	STRPTR	dosName;	//HD0/DF0....
	STRPTR	execName;	//trackdisk.device
	UINT32	unitNum;	// for the ExecDevice
	UINT32	unitFlags;
	struct DosEnvec dosEnv;
};

/* Flags */
#define EBB_CLOGGED	0	/* someone could not be shutup */
#define EBF_CLOGGED	(1<<0)
#define EBB_SHORTMEM	1	/* ran out of expansion mem */
#define EBF_SHORTMEM	(1<<1)
#define EBB_BADMEM	2	/* tried to add bad memory card */
#define EBF_BADMEM	(1<<2)
#define EBB_DOSFLAG	3	/* reserved for use by AmigaDOS */
#define EBF_DOSFLAG	(1<<3)
#define EBB_KICKBACK33	4	/* reserved for use by AmigaDOS */
#define EBF_KICKBACK33	(1<<4)
#define EBB_KICKBACK36	5	/* reserved for use by AmigaDOS */
#define EBF_KICKBACK36	(1<<5)
/* If the following flag is set by a floppy's bootblock code, the initial
   open of the initial shell window will be delayed until the first output
   to that shell.  Otherwise the 1.3 compatible behavior applies. */
#define EBB_SILENTSTART	6
#define EBF_SILENTSTART	(1<<6)


#endif
