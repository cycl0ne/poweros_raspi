#ifndef dos_filesystem_h
#define dos_filesystem_h

#ifndef types_h
#include "types.h"
#endif

struct DosEnvec
{
	UINT32	blockSize;	// 512 normaly, 2048 for CD, 4096 for newer Harddisks
	UINT32	surfaces;
	UINT32	sectorPerBlock;
	UINT32	blockPerTrack;
	UINT32	reservedBlocks;
	UINT32	preAlloc;
	UINT32	lowCyl;
	UINT32	highCyl;
	UINT32	numBuffer;
	UINT32	numBufMemType;
	UINT32	maxTransfer;
	UINT32	memMask;
	UINT32	dosType; // DOS\0, FAT\0, .... Needed for the Handler 0x46415400 (FAT0)
	UINT32	control;
	INT32	bootPri;
	UINT32	bootBlock;
	UINT32	baud;	
};

struct HandlerStartupMsg {
	UINT32			hsm_Unit;	// Unit number for the device
	STRPTR			hsm_Device;	// Exec device name
	struct DosEnvec *hsm_Environ;
	UINT32			hsm_Flags;	// flasg value for the device
};


#endif
