#ifndef Expansion_h
#define Expansion_h

#include "types.h"
#include "dos.h"

struct ExpDosNode
{
	STRPTR	dosName;	//HD0/DF0....
	STRPTR	execName;	//trackdisk.device
	UINT32	unitNum;	// for the ExecDevice
	UINT32	unitFlags;
	struct DosEnvec dosEnv;
};

#endif