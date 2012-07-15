#ifndef devices_h
#define devices_h

#include "types.h"
#include "library.h"
#include "ports.h"

/****** Device ******************************************************/

typedef struct Device {
    struct  Library dd_Library;
} Device;


/****** Unit ********************************************************/

typedef struct Unit {
    struct  MsgPort unit_MsgPort;	/* queue for unprocessed messages */
    UINT8	unit_flags;
    UINT8   unit_pad;
    UINT16  unit_OpenCnt;		/* number of active opens */
} Unit;


#define UNITF_ACTIVE	(1<<0)
#define UNITF_INTASK	(1<<1)

#endif
