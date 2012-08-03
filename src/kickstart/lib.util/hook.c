#include "utilitybase.h"
#include "utility.h"

IPTR util_CallHookPtr(struct UtilityBase *UtilityBase, struct Hook *hook, APTR object, APTR paramPacket)
{
	HOOKFUNC func = (HOOKFUNC)hook->h_Entry;
	return func(hook, object, paramPacket);
}
