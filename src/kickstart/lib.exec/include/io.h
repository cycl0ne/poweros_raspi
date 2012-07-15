#ifndef io_h
#define io_h

#ifndef ports_h
#include "ports.h"
#endif

#ifndef devices_h
#include "devices.h"
#endif

struct IORequest {
    struct  Message io_Message;
    struct  Device  *io_Device;     /* device node pointer  */
    struct  Unit    *io_Unit;	    /* unit (driver private)*/
    UINT16  io_Command;	    /* device command */
    UINT8   io_Flags;
    INT8    io_Error;		    /* error or warning num */
};

typedef struct IOStdReq {
    struct  Message io_Message;
    struct  Device  *io_Device;     /* device node pointer  */
    struct  Unit    *io_Unit;	    /* unit (driver private)*/
    UINT16  io_Command;	    /* device command */
    UINT8   io_Flags;
    INT8    io_Error;		    /* error or warning num */
    UINT32  io_Actual;		    /* actual number of bytes transferred */
    UINT32  io_Length;		    /* requested number bytes transferred*/
    APTR    io_Data;		    /* points to data area */
    UINT32  io_Offset;		    /* offset for block structured devices */
} IOStdReq;

/* library vector offsets for device reserved vectors */
#define DEV_BEGINIO	(-20)
#define DEV_ABORTIO	(-24)

/* io_Flags defined bits */
#define IOB_QUICK	0
#define IOF_QUICK	(1<<0)


#define CMD_INVALID	0
#define CMD_RESET	1
#define CMD_READ	2
#define CMD_WRITE	3
#define CMD_UPDATE	4
#define CMD_CLEAR	5
#define CMD_STOP	6
#define CMD_START	7
#define CMD_FLUSH	8

#define CMD_NONSTD	9

#define IOERR_OPENFAIL	 (-1) /* device/unit failed to open */
#define IOERR_ABORTED	 (-2) /* request terminated early [after AbortIO()] */
#define IOERR_NOCMD		 (-3) /* command not supported by device */
#define IOERR_BADLENGTH	 (-4) /* not a valid length (usually IO_LENGTH) */
#define IOERR_BADADDRESS (-5) /* invalid address (misaligned or bad range) */
#define IOERR_UNITBUSY	 (-6) /* device opens ok, but requested unit is busy */
#define IOERR_SELFTEST	 (-7) /* hardware failed self-test */

#endif
