#include "types.h"
UINT32 __udivsi3(UINT32 n, UINT32 d);
UINT32 __umodsi3(UINT32 a, UINT32 b);

UINT64 LShiftU64(UINT32 op, UINT32 count)
{
	return op << count;
}

UINT64 __aeabi_uldivmod(unsigned numerator, unsigned denominator)
{
	UINT64  Return;
	Return = __udivsi3 (numerator, denominator);
	Return |= LShiftU64 (__umodsi3 (numerator, denominator), 32);
	return Return;
}
