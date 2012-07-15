#include "types.h"
#include "sysbase.h"
#include "memory.h"
#include "ports.h"
#include "device.h"
#include "io.h"
#include "resident.h"
#include "irq.h"

APTR lib_ExpungeLib(struct SysBase *SysBase);
APTR lib_ExtFuncLib(void);
APTR lib_OpenLib(struct SysBase *SysBase);
APTR lib_CloseLib(struct SysBase *SysBase);


void lib_AddHead(SysBase *SysBase, List *list, Node *node);
void lib_AddTail(SysBase *SysBase, List *list, Node *node);
void lib_Enqueue(SysBase *SysBase, List *list, Node *node);
void lib_Insert(SysBase *SysBase, List *list, Node *node, Node *pred);
void lib_NewList(SysBase *SysBase, List *list);
void lib_NewListType(SysBase *SysBase, List *list, UINT8 type);
void lib_Remove(SysBase *SysBase, Node *node);
Node *lib_RemTail(SysBase *SysBase, List *list);
Task *lib_FindTask(SysBase *SysBase, STRPTR name);
struct Node *lib_RemHead(SysBase *SysBase, struct List *list);
struct Node *lib_FindName(SysBase *SysBase, struct List *liste, STRPTR name);
void lib_Enable(struct SysBase *SysBase);
void lib_Disable(struct SysBase *SysBase);
void lib_Permit(struct SysBase *SysBase);
void lib_Forbid(struct SysBase *SysBase);
Task *lib_AddTask(SysBase *SysBase, Task *task, APTR code_start, APTR finalPC, APTR data);

void lib_AddMemList(SysBase *SysBase, UINT32 size, UINT32 attribute, INT32 pri, APTR base, STRPTR name);
APTR lib_Allocate(SysBase *SysBase, struct MemHeader *mh, UINT32 nbytes);
void lib_Deallocate(SysBase *SysBase,struct MemHeader *mh, APTR memoryBlock);
APTR lib_AllocVec(SysBase *SysBase, UINT32 byteSize, UINT32 requirements);
void lib_FreeVec(SysBase *SysBase, APTR memoryBlock);

void lib_Dispatch(void);
void lib_Schedule(SysBase *SysBase);

// Messages
struct Message *lib_GetMsg(SysBase *SysBase, struct MsgPort *msgPort);
void lib_PutMsg(SysBase *SysBase, struct MsgPort *msgPort, struct Message *msg);
void lib_ReplyMsg(SysBase *SysBase, struct Message *msg);
void lib_Signal(SysBase *SysBase, struct Task *task, UINT32 signalSet);
UINT32 lib_Wait(SysBase *SysBase, UINT32 signalSet);

//Devices
INT32 lib_OpenDevice(SysBase *SysBase, STRPTR devName, UINT32 unitNum, struct IORequest *iORequest, UINT32 flags);
void lib_CloseDevice(SysBase *SysBase, struct IORequest *iORequest);
UINT32 lib_RemDevice(SysBase *SysBase, struct Device *device);
void lib_AddDevice(SysBase *SysBase, struct Device *device);

// IO
void lib_AbortIO(SysBase *SysBase, struct IORequest *iORequest);
struct IORequest *lib_CreateIORequest(SysBase *SysBase, struct MsgPort *ioReplyPort, UINT32 size);
void lib_DeleteIORequest(SysBase *SysBase, struct IORequest *iorequest);
struct IORequest *lib_CheckIO(SysBase *SysBase,struct IORequest *iORequest);
INT32 lib_DoIO(SysBase *SysBase, struct IORequest *iORequest);
void lib_SendIO(SysBase *SysBase, struct IORequest *iORequest);
INT32 lib_WaitIO(SysBase *SysBase, struct IORequest *iORequest);

//Ports
void lib_AddPort(SysBase *SysBase, struct MsgPort *msgPort);
struct MsgPort *lib_FindPort(SysBase *SysBase, STRPTR name);
void lib_RemPort(SysBase *SysBase, struct MsgPort *msgPort);
struct Message *lib_WaitPort(SysBase *SysBase, struct MsgPort *msgPort);
struct MsgPort *lib_CreateMsgPort(SysBase *SysBase);
void lib_DeleteMsgPort(SysBase *SysBase, struct MsgPort *msgPort);

// Alocsignal
INT8 lib_AllocSignal(SysBase *SysBase, INT32 signalNum);
void lib_FreeSignal(SysBase *SysBase, INT32 signalNum);


//Library
struct Library *lib_OpenLibrary(SysBase *SysBase, UINT8 *libName, UINT32 version);
void lib_CloseLibrary(SysBase *SysBase,struct Library *library);
void lib_AddLibrary(SysBase *SysBase,struct Library *library);
UINT32 lib_RemLibrary(SysBase *SysBase, struct Library *library);
void lib_SumLibrary(SysBase *SysBase, struct Library *library);

void lib_Alert(SysBase *SysBase, UINT32 alertNum);

struct Library *lib_MakeLibrary(SysBase *SysBase, APTR funcInit, APTR structInit, UINT32(*libInit)(struct Library*,APTR, struct SysBase*), UINT32 dataSize, UINT32 segList);
BOOL lib_RomTagScanner(SysBase *SysBase, UINT32 *start, UINT32 *end);
APTR lib_InitResident(SysBase *SysBase, struct Resident *resident, APTR segList);
void lib_MakeFunctions(SysBase *SysBase, APTR target, APTR functionArray, UINT32 funcDispBase);

INT8 lib_SetTaskPri(SysBase *SysBase, struct Task *task, INT16 pri);

void lib_DPrintF(struct SysBase *SysBase, char *fmt, ...);

int lib_VSNPrintF(struct SysBase *SysBase,char *buf, UINT32 size, const char *fmt, va_list ap);
void lib_VPrintF(struct SysBase *SysBase,const char *fmt, va_list ap);
int lib_SNPrintF(struct SysBase *SysBase,char *buf, UINT32 size, const char *fmt, ...);

//int lib_VSNPrintf(SysBase *SysBase, char *buf, UINT32 size, const char *fmt, va_list ap);
//void lib_DPrintf(SysBase *SysBase,const char *fmt, ...);
//int lib_SNPrintf(SysBase *SysBase,char *buf, UINT32 size, const char *fmt, ...);
void *lib_MemSet(SysBase *SysBase, void* m, int c, UINT32 n);
void *lib_CopyMem(SysBase *SysBase, void* dest, const void* src, int n);

UINT32 lib_Wait(SysBase *SysBase, UINT32 signalSet);
UINT32 lib_SetSignal(SysBase *SysBase, UINT32 newSignals, UINT32 signalSet);
void lib_Signal(SysBase *SysBase, struct Task *task, UINT32 signalSet);
void lib_ReplyMsg(SysBase *SysBase, struct Message *msg);
struct Message *lib_GetMsg(SysBase *SysBase, struct MsgPort *msgPort);
void lib_PutMsg(SysBase *SysBase, struct MsgPort *msgPort, struct Message *msg);

void lib_AddIntServer(SysBase *SysBase, UINT32 intNumber, struct Interrupt *isr);
struct Interrupt *lib_RemIntServer(SysBase *SysBase, UINT32 intNumber, struct Interrupt *isr);
struct Interrupt *lib_CreateIntServer(SysBase *SysBase, const STRPTR name, INT8 pri, APTR handler, APTR data);

void lib_AddExcServer(SysBase *SysBase, UINT32 intNumber, struct Interrupt *isr);
struct Interrupt *lib_RemExcServer(SysBase *SysBase, UINT32 intNumber, struct Interrupt *isr);
