#ifndef dos_h
#define dos_h

#ifndef types_h
#include "types.h"
#endif

#include "tasks.h"
#include "ports.h"
#include "list.h"

#include "dos_packet.h"
#include "dos_action.h"

#ifndef dos_filesystem_h
#include "dos_filesystem.h"
#endif

#define DOSTRUE (-1L)
#define DOSFALSE (0L)

/* definitions for dl_Type */
#define DLT_DEVICE	0
#define DLT_VOLUME	1
#define DLT_DIRECTORY	2	/* assign */
#define DLT_LATE	3	/* late-binding assign */
#define DLT_NONBINDING	4	/* non-binding assign */
#define DLT_PRIVATE	-1	/* for internal use only */


/* Flags to be passed to LockDosList(), etc */
#define LDB_DEVICES	2
#define LDF_DEVICES	(1L << LDB_DEVICES)
#define LDB_VOLUMES	3
#define LDF_VOLUMES	(1L << LDB_VOLUMES)
#define LDB_ASSIGNS	4
#define LDF_ASSIGNS	(1L << LDB_ASSIGNS)
#define LDB_ENTRY	5
#define LDF_ENTRY	(1L << LDB_ENTRY)
#define LDB_DELETE	6
#define LDF_DELETE	(1L << LDB_DELETE)

/* you MUST specify one of LDF_READ or LDF_WRITE */
#define LDB_READ	0
#define LDF_READ	(1L << LDB_READ)
#define LDB_WRITE	1
#define LDF_WRITE	(1L << LDB_WRITE)

/* actually all but LDF_ENTRY (which is used for internal locking) */
#define LDF_ALL		(LDF_DEVICES|LDF_VOLUMES|LDF_ASSIGNS)

// A Handler uses the same structures as the rest of the system + has a port
struct Handler
{
	struct Library han_Library;
	struct MsgPort *han_Port;
};

struct DateStamp
{
	UINT32	ds_Days;
	UINT32	ds_Minute;
	UINT32	ds_Tick;
};

struct AssignList 
{
	struct AssignList *al_Next;
	APTR               al_Lock;
};

struct DeviceNode {
	struct Node		dn_Node;
	struct MsgPort	*dn_Handler;
	struct FileLock	*dn_Lock;
	STRPTR			dn_HandlerName; //Filename to load Handler if HandlerInit = NULL
	APTR			dn_HandlerInit;	//Init Function for Handler
	UINT32			dn_HandlerStack;
	UINT32			dn_HandlerPrio;
	struct HandlerStartupMsg	*dn_HandlerStartupMsg;
};

/* BROKEN!
struct DosList
{
    struct Node      dl_Node;
    struct MsgPort  *dl_Process;
    INT32             dl_Type;
    struct FileLock *dl_Lock;
    union
    {
        struct {
            UINT32             *dl_Handler;
            UINT32              dl_StackSize;
            INT32               dl_Prio;
            struct MinList     dl_LockList;     // List of Locks *internal*
            struct FileSysStartMsg  *dl_Startup;            
        } dl_handler;
        struct {
            struct DateStamp   dl_VolumeDate;
            struct FileLock   *dl_LockList;   //List of Locks
            INT32               dl_DiskType;
        } dl_volume;   
        struct {
            UINT8             *dl_AssignName;
            struct AssignList *dl_List;
        } dl_assign;
    } dl_misc;
};
*/

struct FileLock
{
   struct MinNode	fl_Node;
   struct FileLock	*fl_Lock;
   struct MsgPort	*fl_Process;      // MessagePort to the HandlerProcess
   INT32				fl_Key;
   INT32				fl_Access;
   struct DosList	*fl_Volume;
};

struct FileHandle 
{
   UINT32 fh_Flags;
   INT32 fh_Interactive;
   struct MsgPort *fh_Process;
   APTR fh_Buf;
   INT32 fh_Pos;
   INT32 fh_End;
   INT32 fh_Func1;
   INT32 fh_Func2;
   INT32 fh_Func3;
   INT32 fh_Arg1;
   INT32 fh_Arg2;
   INT32 fh_BufSize;
   INT32 fh_OrigBuf;
}; 

struct Process
{
   struct Task          pr_Task;
   struct MsgPort       pr_MsgPort;
   struct MsgPort       pr_ConsoleTask;
   INT32                 pr_TaskNum;
   INT32                 pr_Result2;
   APTR                 pr_CurrentVolume;
   struct FileLock     *pr_CurrentDir;
   struct FileLock     *pr_HomeDir;
   struct FileHandle   *pr_CIS; // Current Input
   struct FileHandle   *pr_COS; // Current Output
   struct FileHandle   *pr_CER; // Current Error
   APTR                 pr_CLI;
   APTR                 pr_WindowPtr;
   struct MinList       pr_LocalVars;
   INT32                 pr_FLock;
   INT32                 pr_FHandle;
};

/* structure return by GetDeviceProc() */
struct DevProc {
	struct MsgPort *dvp_Port;
	APTR		dvp_Lock;
	UINT32		dvp_Flags;
	struct DosList *dvp_DevNode;	/* DON'T TOUCH OR USE! */
};

/* definitions for dvp_Flags */
#define DVPB_UNLOCK	0
#define DVPF_UNLOCK	(1L << DVPB_UNLOCK)
#define DVPB_ASSIGN	1
#define DVPF_ASSIGN	(1L << DVPB_ASSIGN)

/* These are the return codes used by convention by AmigaDOS commands */
/* See FAILAT and IF for relvance to EXECUTE files		      */
#define RETURN_OK			    0  /* No problems, success */
#define RETURN_WARN			    5  /* A warning only */
#define RETURN_ERROR			   10  /* Something wrong */
#define RETURN_FAIL			   20  /* Complete or severe failure*/

/* Bit numbers that signal you that a user has issued a break */
#define SIGBREAKB_CTRL_C   12
#define SIGBREAKB_CTRL_D   13
#define SIGBREAKB_CTRL_E   14
#define SIGBREAKB_CTRL_F   15

/* Bit fields that signal you that a user has issued a break */
/* for example:	 if (SetSignal(0,0) & SIGBREAKF_CTRL_C) cleanup_and_exit(); */
#define SIGBREAKF_CTRL_C   (1<<SIGBREAKB_CTRL_C)
#define SIGBREAKF_CTRL_D   (1<<SIGBREAKB_CTRL_D)
#define SIGBREAKF_CTRL_E   (1<<SIGBREAKB_CTRL_E)
#define SIGBREAKF_CTRL_F   ((long)1<<SIGBREAKB_CTRL_F)

/* Values returned by SameLock() */
#define LOCK_DIFFERENT		-1
#define LOCK_SAME		0
#define LOCK_SAME_VOLUME	1	/* locks are on same volume */
#define LOCK_SAME_HANDLER	LOCK_SAME_VOLUME
/* LOCK_SAME_HANDLER was a misleading name, def kept for src compatibility */

/* types for ChangeMode() */
#define CHANGE_LOCK	0
#define CHANGE_FH	1

/* Values for MakeLink() */
#define LINK_HARD	0
#define LINK_SOFT	1	/* softlinks are not fully supported yet */

/* values returned by ReadItem */
#define	ITEM_EQUAL	-2		/* "=" Symbol */
#define ITEM_ERROR	-1		/* error */
#define ITEM_NOTHING	0		/* *N, ;, endstreamch */
#define ITEM_UNQUOTED	1		/* unquoted item */
#define ITEM_QUOTED	2		/* quoted item */

/* types for AllocDosObject/FreeDosObject */
#define DOS_FILEHANDLE		0	/* few people should use this */
#define DOS_EXALLCONTROL	1	/* Must be used to allocate this! */
#define	DOS_FIB			2	/* useful */
#define DOS_STDPKT		3	/* for doing packet-level I/O */
#define DOS_CLI			4	/* for shell-writers, etc */
#define DOS_RDARGS		5	/* for ReadArgs if you pass it in */

#endif
