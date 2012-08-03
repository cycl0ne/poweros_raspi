#include "types.h"
#include "tasks.h"
#include "sysbase.h"
#include "context.h"
#include "exec_funcs.h"


void lib_UART_send ( unsigned int c );

void lib_Print_uart0(const char *s);

void __div0 (void)
{
	lib_Print_uart0("Division by 0\n");
	for(;;);
}

void lib_DMB(void) 
{
	UINT32 name = 0; 
	__asm__ __volatile__ ("mcr	p15,0,%[t],c7,c10,5\n" :: [t] "r" (name) : "memory");
}

void lib_DSB(void) 
{
	UINT32 name = 0; 
	__asm__ __volatile__ ("mcr	p15,0,%[t],c7,c10,4\n" :: [t] "r" (name) : "memory");
}

void *asm_memcpy(void *dest, const void *src, size_t n);

void *lib_CopyMemQuick(SysBase *SysBase, const APTR src, APTR dest, int n) 
{
	const UINT32 *f = src;
	UINT32 *t = dest;
	while (n-- >0) *t++ = *f++;
	return dest;
}

void *lib_CopyMem(SysBase *SysBase,const APTR src,  APTR dest, int n) 
{
/*
    const char *f = src;
    char *t = dest;
    while (n-- > 0) *t++ = *f++;
    return dest;
*/
	asm_memcpy(dest, src, n); // fix, should asm_ return a value?
	return dest;
}

extern void *asm_memset(void* m, int c, UINT32 n);

void *lib_MemSet(SysBase *SysBase, void* m, int c, UINT32 len) 
{
//	DPrintF("[MemSet] Clear Memory: %x with %x len %x", m, c, len);
//	char *bb;
//	for (bb = (char *)m; len--; ) *bb++ = c;
	return asm_memset(m, c, len);
	//DPrintF("[MemSet] Clear Memory: %x with %x len %x", m, c, len);
}

void DebugPutS(const INT8 *s)
{
	while(*s != '\0') 
	{
		lib_UART_send(*s);
		s++;
	}
}

static void RawSerIo(INT32 c, APTR Data)
{
	lib_UART_send((UINT32)c);
}

void lib_DPrintF(struct SysBase *SysBase, char *fmt, ...)
{
//    char buf[128];
    va_list pvar;
    va_start(pvar,fmt);
//    RawDoFmt(fmt, pvar, RawSerIo, NULL);
//	lib_Print_uart0("VFPRINT\n");
	VPrintF(fmt, pvar);
	va_end(pvar);
}

