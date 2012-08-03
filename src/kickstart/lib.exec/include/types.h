/*
*	types.h
*	
* Defines for Types
*
*/

#ifndef types_h
#define types_h

typedef void				VOID;
typedef void *				APTR;

typedef signed char			INT8;
typedef signed short		INT16;
typedef signed int			INT32;
typedef signed long long int	INT64;

typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;
typedef unsigned long long int	UINT64;

typedef float				FLOAT;
typedef	double				DOUBLE;

typedef volatile signed char			VINT8;
typedef volatile signed short int		VINT16;
typedef volatile signed int				VINT32;
typedef volatile signed long long int	VINT64;

typedef volatile unsigned char			VUINT8;
typedef volatile unsigned short int		VUINT16;
typedef volatile unsigned int			VUINT32;
typedef volatile unsigned long long int	VUINT64;


typedef UINT32				UINTPTR;

typedef unsigned int		IPTR;
typedef unsigned char*		STRPTR;
typedef unsigned int		BOOL;

#define NULL				0
#define TRUE				1
#define FALSE				0

#define ABS(a)				((a)>0?(a):-(a))
#define MIN(a,b)			((a)<(b)?(a):(b))
#define MAX(a,b)			((a)>(b)?(a):(b))
#define CLAMP(min,x,max)	((x)>(max)?(max):((x)<(min)?(min):(x)))

#define SET_BITS(data, mask)	((data)|=(mask))
#define CLEAR_BITS(data, mask)	((data)&=(~(mask)))
#define CHANGE_BITS(data, mask)	((data)^=(mask))
#define TEST_BITS(data, mask)	(((data)&(mask)) != 0)
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define DMB_IMPL(name) { UINT32 name = 0; __asm__ __volatile__ ("mcr	p15,0,%[t],c7,c10,5\n" :: [t] "r" (name) : "memory"); }
#define DSB_IMPL(name) { UINT32 name = 0; __asm__ __volatile__ ("mcr	p15,0,%[t],c7,c10,4\n" :: [t] "r" (name) : "memory"); }
#define DMB DMB_IMPL(MACRO_CONCAT(__dmb_, __COUNTER__))
#define DSB DSB_IMPL(MACRO_CONCAT(__dsb_, __COUNTER__))

#define READ32(addr) (*(VUINT32*)(addr))
#define READ16(addr) (*(VUINT16*)(addr))
#define READ8(addr) (*(VUINT8*)(addr))

#define WRITE32(addr,value) (*(VUINT32*)(addr) = (VUINT32)(value))
#define WRITE16(addr,value) (*(VUINT16*)(addr) = (VUINT16)(value))
#define WRITE8(addr,value) (*(VUINT8*)(addr) = (VUINT8)(value))

#if 0
#define WRITE8(c,v)		({ UINT8  __v = v; DMB; (*(VUINT8*)(c)  = (VUINT8)(v)); __v; })
#define WRITE16(c,v)	({ UINT16 __v = v; DMB; (*(VUINT16*)(c) = (VUINT16)(v)); __v; })
#define WRITE32(c,v)	({ UINT32 __v = v; DMB; (*(VUINT32*)(c) = (VUINT32)(v)); __v; })

#define READ8(c)	({ UINT8  __v = (*(VUINT8*)(c)); DMB; __v; })
#define READ16(c)	({ UINT16 __v = (*(VUINT16*)(c)); DMB; __v; })
#define READ32(c)	({ UINT32 __v = (*(VUINT32*)(c)); DMB; __v; })
#endif

#define FADDR(fptr)  ((APTR) (fptr))

struct JumpVec
{
	VUINT32	vec;
};

#define _GET_VEC(v) ((APTR)(*(UINT32*)(v)))
#define _GETJUMPVEC(lib,n)  ((struct JumpVec *)(((UINT8 *)lib)-((n)*4)))
#define _GETVECADDR(lib,n)  (_GET_VEC(_GETJUMPVEC(lib,n)))

#define __GNUC_VA_LIST
typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;

#define va_start(v,l)	__builtin_va_start(v,l)
#define va_end(v)	__builtin_va_end(v)
#define va_arg(v,l)	__builtin_va_arg(v,l)

/*
typedef char *			va_list;

#define	__va_size(type) \
	(((sizeof(type) + sizeof(long) - 1) / sizeof(long)) * sizeof(long))

#define	va_start(ap, last) \
	((ap) = (va_list)&(last) + __va_size(last))

#define	va_arg(ap, type) \
	(*(type *)(void *)((ap) += __va_size(type), (ap) - __va_size(type)))

#define	va_end(ap)

#define	va_copy(dest, src)	((dest) = (src))
*/
#endif
