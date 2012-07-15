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

void *lib_CopyMem(SysBase *SysBase, void* dest, const void* src, int n) 
{
    const char *f = src;
    char *t = dest;

    while (n-- > 0) *t++ = *f++;
    return dest;
}

extern void *asm_memset(void* m, int c, UINT32 n);

void *lib_MemSet(SysBase *SysBase, void* m, int c, UINT32 len) 
{
//	DPrintF("[MemSet] Clear Memory: %x with %x len %x", m, c, len);
//	char *bb;
//	for (bb = (char *)m; len--; ) *bb++ = c;
	asm_memset(m, c, len);
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

void lib_DPrintF(struct SysBase *SysBase, char *fmt, ...)
{
//    char buf[128];
    va_list pvar;
    va_start(pvar,fmt);
    VPrintF(fmt, pvar);
    va_end(pvar);
}

