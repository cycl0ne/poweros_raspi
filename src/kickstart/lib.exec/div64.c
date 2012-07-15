#include "types.h"

UINT32 __div64_32(UINT64 *n, UINT32 base)
{
	UINT64 rem = *n;
	UINT64 b = base;
	UINT64 res, d = 1;
	UINT32 high = rem >> 32;

	/* Reduce the thing a bit first */
	res = 0;
	if (high >= base) {
		high /= base;
		res = (UINT64) high << 32;
		rem -= (UINT64) (high*base) << 32;
	}

	while ((INT64)b > 0 && b < rem) {
		b = b+b;
		d = d+d;
	}

	do {
		if (rem >= b) {
			rem -= b;
			res += d;
		}
		b >>= 1;
		d >>= 1;
	} while (d);

	*n = res;
	return rem;
}
