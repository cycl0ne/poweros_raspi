#ifndef utilitybase_h
#define utilitybase_h

#include "types.h"
#include "library.h"
#include "semaphores.h"

struct NameSpace
{
    struct MinList          ns_List;
    struct SignalSemaphore  ns_Lock;
    UINT32                   ns_Flags;
};

struct UtilityBase
{
	struct Library		ut_lib;
	APTR				ut_SysBase;
    UINT32				ut_LastID;
    struct NameSpace	ut_NameSpace;

};

#endif
