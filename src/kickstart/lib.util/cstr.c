#include "utilitybase.h"
#include "utility_funcs.h"

#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

static const unsigned char _ctype[] = {
_C,_C,_C,_C,_C,_C,_C,_C,			/* 0-7 */
_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,		/* 8-15 */
_C,_C,_C,_C,_C,_C,_C,_C,			/* 16-23 */
_C,_C,_C,_C,_C,_C,_C,_C,			/* 24-31 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,			/* 32-39 */
_P,_P,_P,_P,_P,_P,_P,_P,			/* 40-47 */
_D,_D,_D,_D,_D,_D,_D,_D,			/* 48-55 */
_D,_D,_P,_P,_P,_P,_P,_P,			/* 56-63 */
_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,	/* 64-71 */
_U,_U,_U,_U,_U,_U,_U,_U,			/* 72-79 */
_U,_U,_U,_U,_U,_U,_U,_U,			/* 80-87 */
_U,_U,_U,_P,_P,_P,_P,_P,			/* 88-95 */
_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,	/* 96-103 */
_L,_L,_L,_L,_L,_L,_L,_L,			/* 104-111 */
_L,_L,_L,_L,_L,_L,_L,_L,			/* 112-119 */
_L,_L,_L,_P,_P,_P,_P,_C,			/* 120-127 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 128-143 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 144-159 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,   /* 160-175 */
_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,       /* 176-191 */
_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,       /* 192-207 */
_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,       /* 208-223 */
_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,       /* 224-239 */
_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};      /* 240-255 */

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define isalnum(c)	((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)	((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)
#define isdigit(c)	((__ismask(c)&(_D)) != 0)
#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)

/*
 * Rather than doubling the size of the _ctype lookup table to hold a 'blank'
 * flag, just check for space or tab.
 */
#define isblank(c)	(c == ' ' || c == '\t')

#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

UINT32 util_IsLower(struct UtilityBase *UtilityBase, UINT32 c)
{
	return islower(c);
}

UINT32 util_IsUpper(struct UtilityBase *UtilityBase, UINT32 c)
{
	return isupper(c);
}

UINT32 util_ToUpper(struct UtilityBase *UtilityBase, UINT32 c)
{
	return islower(c) ? c - 'a' + 'A' : c;
}

UINT32 util_ToLower(struct UtilityBase *UtilityBase, UINT32 c)
{

  return isupper(c) ? (c) - 'A' + 'a' : c;
}

UINT32 util_Strlen(struct UtilityBase *UtilityBase, const char * s)
{
	const char *sc;
	for (sc = s; *sc != '\0'; ++sc)/* nothing */;
	return sc - s;
}

UINT32 util_Strnlen(struct UtilityBase *UtilityBase, char const *s, UINT32 count)
{
	const char *sc;

	for(sc = s; count-- && *sc != '\0'; ++sc);
	return sc - s;
}

UINT32 util_Strncasecmp(struct UtilityBase *UtilityBase, const char *s1, const char *s2, UINT32 n)
{
	const unsigned char *ucs1 = (const unsigned char *) s1;
	const unsigned char *ucs2 = (const unsigned char *) s2;
	int d = 0;
	for ( ; n != 0; n--)
	{
		const int c1 = ToLower(*ucs1++);
		const int c2 = ToLower(*ucs2++);
		if (((d = c1 - c2) != 0) || (c2 == '\0')) break;
	}
	return d;
}

UINT32 util_Stricmp(struct UtilityBase *UtilityBase, const char *s1, const char *s2)
{
	const unsigned char *ucs1 = (const unsigned char *) s1;
	const unsigned char *ucs2 = (const unsigned char *) s2;
	int d = 0;
	for ( ; ; )
	{
		const int c1 = ToLower(*ucs1++);
		const int c2 = ToLower(*ucs2++);
		if (((d = c1 - c2) != 0) || (c2 == '\0')) break;
	}
	return d;
}

UINT32 util_Strnicmp(struct UtilityBase *UtilityBase, const char *s1, const char *s2, UINT32 n)
{
	const unsigned char *ucs1 = (const unsigned char *) s1;
	const unsigned char *ucs2 = (const unsigned char *) s2;
	int d = 0;
	for ( ; n != 0; n--)
	{
		const int c1 = ToLower(*ucs1++);
		const int c2 = ToLower(*ucs2++);
		if (((d = c1 - c2) != 0) || (c2 == '\0')) break;
	}
	return d;
}


char *util_Strchr(struct UtilityBase *UtilityBase, const char *s, int c)
{
	while (*s && *s != c)
	s++;
	if (*s == c) return (char *)s;
	return NULL;
}

char* util_Strcpy(struct UtilityBase *UtilityBase, char *dst0, const char *src0)
{
	char *s = dst0;
	while (*dst0++ = *src0++);

	return s;
}

char* util_Strrchr(struct UtilityBase *UtilityBase, const char *s, int i)
{
	const char *last = NULL;

	if (i)
	{
		while ((s=Strchr(s, i)))
		{
			last = s;
			s++;
		}
	} else
	{
		last = Strchr(s, i);
	}
	return (char *) last;
}

char *util_Strcat(struct UtilityBase *UtilityBase, char *s1, const char *s2)
{
  char *s = s1;

  while (*s1) s1++;
  while (*s1++ = *s2++);
  return s;
}

