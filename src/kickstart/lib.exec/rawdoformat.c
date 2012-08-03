#include "types.h"
#include "sysbase.h"
#include "exec_funcs.h"

static void lib_int_RawDoFmtNumber(UINT32 ul, int base, void (*PutCh)(INT32, APTR), APTR PutChData) 
{
	// hold a long in base 8
	char *p, buf[(sizeof(long) * 8 / 3) + 1];
	
	p = buf;
	
	do {
		*p++ = "0123456789abcdef"[ul % base];
	} while (ul /= base);
	
	do {
		PutCh(*--p, PutChData);
	} while (p > buf);
}

va_list lib_RawDoFmt(struct SysBase *SysBase, const char *fmt, va_list ap, void (*PutCh)(INT32, APTR), APTR PutChData)
{	
	char *p;
	int ch, n;
	unsigned long ul;
	int lflag, set;
	
	for (;;) {
		while ((ch = *fmt++) != '%') {
			if (ch == '\0') 
			{
				PutCh(ch, PutChData); // report the \0
				return ap;
			}
			PutCh(ch, PutChData);
		}
		lflag = 0;
		
reswitch:
	
	switch (ch = *fmt++) {
		case '\0':
			PutCh(ch, PutChData); // report the \0			
			return ap;
		
		case 'l':
			lflag = 1;
			goto reswitch;
		
		case 'B':
		case 'b':
			ul = va_arg(ap, int);
			p = va_arg(ap, char *);
			lib_int_RawDoFmtNumber(ul, *p++, PutCh, PutChData);
			if (!ul)
				break;
			
			for (set = 0; (n = *p++) != 0;) {
				if (ul & (1 << (n - 1))) {
					PutCh(set ? ',' : '<', PutChData);
					for (; (n = *p) > ' '; ++p)
						PutCh(n, PutChData);
					set = 1;
				} else
					for (; *p > ' '; ++p);
			}
			if (set)
				PutCh('>', PutChData);
			break;
		
		case 'C':
		case 'c':
			ch = va_arg(ap, int);
				PutCh(ch & 0x7f, PutChData);
			break;
		
		case 'S':
		case 's':
			p = va_arg(ap, char *);
			while ((ch = *p++) != 0)
				PutCh(ch, PutChData);
			break;
		
		case 'D':
		case 'd':
			ul = lflag ? va_arg(ap, long) : va_arg(ap, int);
			if ((long)ul < 0) {
				PutCh('-', PutChData);
				ul = -(long)ul;
			}
			lib_int_RawDoFmtNumber(ul, 10, PutCh, PutChData);
			break;
		
		case 'O':
		case 'o':
			ul = va_arg(ap, UINT32);
			lib_int_RawDoFmtNumber(ul, 8, PutCh, PutChData);
			break;
		
		case 'U':
		case 'u':
			ul = va_arg(ap, UINT32);
			lib_int_RawDoFmtNumber(ul, 10, PutCh, PutChData);
			break;
		
		case 'P':
		case 'p':
			PutCh('0', PutChData);
			PutCh('x', PutChData);
			lflag = 1;
			// fall through
		case 'X':
		case 'x':
			ul = va_arg(ap, UINT32);
			lib_int_RawDoFmtNumber(ul, 16, PutCh, PutChData);
			break;
		
		default:
			PutCh('%', PutChData);
			if (lflag)
				PutCh('l', PutChData);
			PutCh(ch, PutChData);
		}
	}
	va_end(ap);
	return ap;
}

