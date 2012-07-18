#ifndef testdev_h
#define testdev_h

#include "types.h"
#include "device.h"
#include "io.h"

#define F_SHUTDOWN 0x1

typedef struct TestDevBase
{
	struct Device	Device;
	UINT32			Flags;
	APTR			SysBase;
	struct Task			*Task;
    struct MsgPort      *TaskPort;
    struct MsgPort      *TimerPort;

	APTR				Timer_Dev;
    struct TimeRequest  *Timer_Unit;
	UINT32				Timer;
}TestDevBase;


#endif
