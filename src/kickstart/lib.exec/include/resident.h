#ifndef resident_h
#define resident_h

#include "types.h"

struct Resident
{
    UINT32            rt_MatchWord; /* equal to RTC_MATCHWORD (see below) */
    struct Resident *rt_MatchTag;  /* Pointer to this struct */
    APTR             rt_EndSkip;
    UINT8            rt_Flags;     /* see below */
    UINT8            rt_Version;
    UINT8            rt_Type;
    INT8             rt_Pri;
    STRPTR				rt_Name;
    STRPTR				rt_IdString;
    UINT16				rt_Padding;
    APTR				rt_Init;
};

struct ResidentNode
{
    struct Node      rn_Node;
    struct Resident *rn_Resident;
};

#define RTC_MATCHWORD 0x0DEC0DE0

#define RTF_COLDSTART  (1<<0)
#define RTF_SINGLETASK (1<<1)
#define RTF_AFTERDOS   (1<<2)
#define RTF_AUTOINIT   (1<<7)

#define RTW_NEVER     0
#define RTW_COLDSTART 1



#endif
