#include "usbbase_internal.h"

#define STC_FREQ_HZ  1000000
#include "raspi_platform.h"

#include "exec_funcs.h"

struct UsbBase *UsbBase_;

#ifdef SysBase
#undef SysBase
#define SysBase UsbBase_->USB_SysBase
#endif 

int memcmp(const void *s1, const void *s2, size_t n)
{
	if (n != 0) {
	const unsigned char *p1 = s1, *p2 = s2;
	do {
		if (*p1++ != *p2++) return (*--p1 - *--p2);
		} while (--n != 0);
	}
	return (0);
}

void sprintf(char *buf, const char *fmt, ...)
{
    va_list pvar;
    va_start(pvar,fmt);
    VSNPrintF(buf, 40, fmt, pvar);
    va_end(pvar);
}

void printf(const char *fmt, ...)
{
    va_list pvar;
    va_start(pvar,fmt);
    VPrintF(fmt, pvar);
    va_end(pvar);
}

APTR malloc(UINT32 byteSize)
{
	return AllocVec(byteSize, MEMF_FAST|MEMF_CLEAR);
}

void free(APTR mem)
{
	FreeVec(mem);
}

void *memcpy(void* dest, void* src, int n)
{
	return CopyMem(dest, src, n);
}

#if 0
void memset(void *b, int c, UINT32 len)
{
	char *bb;
//	if (c == 0) bzero(b, len);
//	else
	for (bb = (char *)b; len--; ) *bb++ = c;
//	return (b);
}
#endif

void memset(void* m, int c, UINT32 n)
{
	MemSet(m, c, n);
}

UINT32 get_timer(UINT32 base)
{
	return READ32(ST_BASE+0x04)-base;
}

void __udelay(UINT32 usec)
{
	UINT32	tmo, endtime;
	INT32	diff;
	tmo = usec;
	endtime = READ32(ST_BASE+0x04) + tmo;
	do {
		UINT32 now = READ32(ST_BASE+0x04);
		diff = endtime - now;
	} while (diff >=0);
}

void udelay(UINT32 delay)
{
	__udelay(delay);
}

void mdelay(UINT32 msec)
{
	while (msec--) udelay(1000);
}
