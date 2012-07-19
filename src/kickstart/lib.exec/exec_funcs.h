#include "types.h"
#include "memory.h"
#include "device.h"
#include "io.h"
#include "resident.h"
#include "irq.h"
#include "tasks.h"

#define size_t UINT32


void Permit();
void Forbid();
void Enable(UINT32);
UINT32 Disable();

void AddHead( List *list, Node *node);
void AddTail( List *list, Node *node);
void Enqueue( List *list, Node *node);
void Insert( List *list, Node *node, Node *pred);
void NewList( List *list);
void NewListType( List *list, UINT8 type);
void Remove( Node *node);
Node *RemTail( List *list);
Task *AddTask(Task *task, APTR code_start, APTR finalPC, APTR data);

void AddMemList(UINT32 size, UINT32 attribute, INT32 pri, APTR base, STRPTR name);
APTR Allocate(struct MemHeader *mh, UINT32 nbytes);
void Deallocate(struct MemHeader *mh, APTR memoryBlock);
APTR AllocVec(UINT32 byteSize, UINT32 requirements);
void FreeVec(APTR memoryBlock);
void Schedule();
void Dispatch();

// Messages
struct Message *GetMsg( struct MsgPort *msgPort);
void PutMsg( struct MsgPort *msgPort, struct Message *msg);
void ReplyMsg( struct Message *msg);
void Signal( struct Task *task, UINT32 signalSet);
UINT32 Wait( UINT32 signalSet);

//Devices
INT32 OpenDevice(STRPTR devName, UINT32 unitNum, struct IORequest *iORequest, UINT32 flags);
void CloseDevice( struct IORequest *iORequest);
UINT32 RemDevice( struct Device *device);
void AddDevice( struct Device *device);

// IO
void AbortIO( struct IORequest *iORequest);
struct IORequest *CreateIORequest( struct MsgPort *ioReplyPort, UINT32 size);
void DeleteIORequest( struct IORequest *iorequest);
struct IORequest *CheckIO(struct IORequest *iORequest);
INT32 DoIO( struct IORequest *iORequest);
void SendIO( struct IORequest *iORequest);
INT32 WaitIO( struct IORequest *iORequest);

//Ports
void AddPort( struct MsgPort *msgPort);
struct MsgPort *FindPort( STRPTR name);
void RemPort( struct MsgPort *msgPort);
struct Message *WaitPort( struct MsgPort *msgPort);
struct MsgPort *CreateMsgPort();
void DeleteMsgPort( struct MsgPort *msgPort);

INT8 AllocSignal(INT32 signalNum);
void FreeSignal(INT32 signalNum);

struct Library *OpenLibrary( UINT8 *libName, UINT32 version);
void CloseLibrary(struct Library *library);
void AddLibrary(struct Library *library);
UINT32 RemLibrary( struct Library *library);
void SumLibrary( struct Library *library);
void Alert(UINT32 alertNum);

struct Library *MakeLibrary(APTR funcInit, APTR structInit, UINT32(*libInit)(struct Library*,APTR, APTR), UINT32 dataSize, UINT32 segList);
BOOL RomTagScanner(UINT32 *start, UINT32 *end);
APTR InitResident(struct Resident *resident, APTR segList);
void MakeFunctions(APTR target, APTR functionArray, UINT32);
INT8 SetTaskPri(struct Task *task, INT16 pri);

void  DPrintF(char *fmt, ...);

int VSNPrintF(char *buf, size_t size, const char *fmt, va_list ap);
void VPrintF(const char *fmt, va_list ap);
int SNPrintF(char *buf, size_t size, const char *fmt, ...);

void *MemSet(void* m, int c, UINT32 n);
void *CopyMem(void* dest, const void* src, int n);

UINT32 SetSignal(UINT32 newSignals, UINT32 signalSet);

void AddIntServer(UINT32 intNumber, struct Interrupt *isr);
struct Interrupt *RemIntServer(UINT32 intNumber, struct Interrupt *isr);
struct Interrupt *CreateIntServer(const STRPTR name, INT8 pri, APTR handler, APTR data);
void AddExcServer(UINT32 intNumber, struct Interrupt *isr);
struct Interrupt *RemExcServer(UINT32 intNumber, struct Interrupt *isr);

Task *TaskCreate(const char *name, APTR codeStart, APTR data, UINT32 stackSize, INT8 pri);

void InitSemaphore(struct SignalSemaphore *signalSemaphore);
void AddSemaphore(const char *semName, struct SignalSemaphore *sigSem);
void RemSemaphore(struct SignalSemaphore *sigSem);
struct SignalSemaphore *FindSemaphore(const char *name);

void ObtainSemaphore(struct SignalSemaphore *sigSem);
BOOL AttemptSemaphore(struct SignalSemaphore *signalSemaphore);
void ReleaseSemaphore(struct SignalSemaphore *sigSem);


#define OpenLib(x)  	(((APTR(*)(APTR,APTR)) _GETVECADDR(SysBase,1))(SysBase,x))
#define CloseLib(x) 	(((APTR(*)(APTR,APTR)) _GETVECADDR(SysBase,2))(SysBase,x))
#define ExpungeLib(x)	(((APTR(*)(APTR,APTR)) _GETVECADDR(SysBase,3))(SysBase,x))
#define ExtFuncLib(x)	(((APTR(*)(APTR,APTR)) _GETVECADDR(SysBase,4))(SysBase,x))

// Ables
#define Permit()		(((APTR(*)(APTR)) _GETVECADDR(SysBase,5))(SysBase))
#define Forbid()		(((APTR(*)(APTR)) _GETVECADDR(SysBase,6))(SysBase))
#define Enable(a)		(((VOID(*)(APTR, UINT32)) _GETVECADDR(SysBase,7))(SysBase, a))
#define Disable()		(((UINT32(*)(APTR)) _GETVECADDR(SysBase,8))(SysBase))


#define NewList(x)		(((void(*)(APTR, struct List *)) 								_GETVECADDR(SysBase, 9))(SysBase,x))
#define Enqueue(x,y)	(((void(*)(APTR, struct List *, struct Node *))					_GETVECADDR(SysBase,10))(SysBase,x,y))
#define FindName(x,y)	(((struct Node *(*)(APTR, struct List *, UINT8 *))				_GETVECADDR(SysBase,11))(SysBase,x,y))
#define RemTail(x)		(((struct Node *(*)(APTR, struct List *))						_GETVECADDR(SysBase,12))(SysBase,x))
#define AddTail(x,y)	(((void(*)(APTR, struct List *, struct Node *))					_GETVECADDR(SysBase,13))(SysBase,x,y))
#define	RemHead(x)		(((struct Node *(*)(APTR, struct List *))						_GETVECADDR(SysBase,14))(SysBase,x))
#define	AddHead(x,y)	(((void(*)(APTR, struct List *, struct Node *))					_GETVECADDR(SysBase,15))(SysBase,x,y))
#define	Remove(x)		(((void(*)(APTR, struct Node *))								_GETVECADDR(SysBase,16))(SysBase,x))
#define	Insert(x,y,z)	(((void(*)(APTR, struct List *, struct Node *, struct Node *))	_GETVECADDR(SysBase,17))(SysBase,x,y,z))
#define NewListType(x,y)		(((void(*)(APTR, struct List *, UINT8)) _GETVECADDR(SysBase, 18))(SysBase,x,y))

#define	FindTask(a)     	(((struct Task*(*)(APTR, UINT8 * ))             				_GETVECADDR(SysBase, 19))(SysBase,a))
#define	AddTask(a,b,c,d)	(((struct Task*(*)(APTR, Task *, APTR, APTR, APTR))         _GETVECADDR(SysBase, 20))(SysBase,a,b,c,d))

#define	AddMemList(a,b,c,d,e)	(((void(*)(APTR, UINT32, UINT32, INT32, APTR, STRPTR))	_GETVECADDR(SysBase, 21))(SysBase,a,b,c,d,e))
#define Allocate(x,y)			(((APTR(*)(APTR, MemHeader *, UINT32))					_GETVECADDR(SysBase, 22))(SysBase,x,y))
#define Deallocate(x,y)			(((void(*)(APTR, MemHeader *, APTR))					_GETVECADDR(SysBase, 23))(SysBase,x,y))
#define AllocVec(x,y)			(((APTR(*)(APTR, UINT32, UINT32))						_GETVECADDR(SysBase, 24))(SysBase,x,y))
#define FreeVec(x)				(((void(*)(APTR, APTR))									_GETVECADDR(SysBase, 25))(SysBase,x))

#define Schedule()				(((void(*)(APTR))										_GETVECADDR(SysBase, 26))(SysBase))
#define Dispatch()				(((void(*)(APTR))										_GETVECADDR(SysBase, 27))(SysBase))

#define GetMsg(x)				(((struct Message*(*)(APTR, struct MsgPort*))			_GETVECADDR(SysBase, 28))(SysBase,x))
#define PutMsg(x,y)				(((void(*)(APTR, struct MsgPort*, struct Message*))		_GETVECADDR(SysBase, 29))(SysBase,x,y))
#define ReplyMsg(x)				(((void(*)(APTR, struct Message*))						_GETVECADDR(SysBase, 30))(SysBase,x))
#define Signal(x,y)				(((void(*)(APTR, struct Task*, UINT32))					_GETVECADDR(SysBase, 31))(SysBase,x, y))
#define Wait(x)					(((UINT32(*)(APTR, UINT32))								_GETVECADDR(SysBase, 32))(SysBase,x))

#define OpenDevice(x,y,z,a)		(((INT32(*)(APTR, STRPTR, UINT32, struct IORequest*, UINT32))	_GETVECADDR(SysBase, 33))(SysBase,x,y,z,a))
#define CloseDevice(x)			(((void(*)(APTR, struct IORequest*))							_GETVECADDR(SysBase, 34))(SysBase,x))
#define RemDevice(x)			(((UINT32(*)(APTR, struct Device *))							_GETVECADDR(SysBase, 35))(SysBase,x))
#define AddDevice(x)			(((void(*)(APTR, struct Device *))								_GETVECADDR(SysBase, 36))(SysBase,x))

#define AbortIO(x)			(((void(*)(APTR, struct IORequest*))							_GETVECADDR(SysBase, 37))(SysBase,x))
#define CreateIORequest(x,y)	(((APTR(*)(APTR, struct MsgPort*, UINT32))	_GETVECADDR(SysBase, 38))(SysBase,x,y))
#define DeleteIORequest(x)	(((void(*)(APTR, struct IORequest*))							_GETVECADDR(SysBase, 39))(SysBase,x))
#define CheckIO(x)			(((struct IORequest *(*)(APTR, struct IORequest*))				_GETVECADDR(SysBase, 40))(SysBase,x))
#define DoIO(x)				(((INT32(*)(APTR, struct IORequest*))							_GETVECADDR(SysBase, 41))(SysBase,x))
#define SendIO(x)			(((void(*)(APTR, struct IORequest*))							_GETVECADDR(SysBase, 42))(SysBase,x))
#define WaitIO(x)			(((INT32(*)(APTR, struct IORequest*))							_GETVECADDR(SysBase, 43))(SysBase,x))

#define AddPort(x)			(((void(*)(APTR, struct MsgPort *))					_GETVECADDR(SysBase, 44))(SysBase,x))
#define FindPort(x)			(((struct MsgPort *(*)(APTR, STRPTR))				_GETVECADDR(SysBase, 45))(SysBase,x))
#define RemPort(x)			(((void(*)(APTR, struct MsgPort *))					_GETVECADDR(SysBase, 46))(SysBase,x))
#define WaitPort(x)			(((struct Message *(*)(APTR, struct MsgPort *))		_GETVECADDR(SysBase, 47))(SysBase,x))
#define CreateMsgPort()	(((struct MsgPort *(*)(APTR))						_GETVECADDR(SysBase, 48))(SysBase))
#define DeleteMsgPort(x)	(((void(*)(APTR, struct MsgPort *))					_GETVECADDR(SysBase, 49))(SysBase,x))

#define AllocSignal(x)			(((INT8(*)(APTR, INT32))					_GETVECADDR(SysBase, 50))(SysBase,x))
#define FreeSignal(x)			(((INT8(*)(APTR, INT32))					_GETVECADDR(SysBase, 51))(SysBase,x))

#define OpenLibrary(x,y)		(((struct Library*(*)(APTR, STRPTR, UINT32))	_GETVECADDR(SysBase, 52))(SysBase,x, y))
#define CloseLibrary(x)			(((void(*)(APTR, struct Library *))				_GETVECADDR(SysBase, 53))(SysBase,x))
#define AddLibrary(x)			(((void(*)(APTR, struct Library *))				_GETVECADDR(SysBase, 54))(SysBase,x))
#define RemLibrary(x)			(((INT32(*)(APTR, struct Library *))			_GETVECADDR(SysBase, 55))(SysBase,x))
#define SumLibrary(x)			(((void(*)(APTR, struct Library *))				_GETVECADDR(SysBase, 56))(SysBase,x))

#define Alert(x)			(((void(*)(APTR, UINT32))				_GETVECADDR(SysBase, 57))(SysBase,x))

#define  MakeLibrary(a,b,c,d,e)  (((struct Library *(*)(APTR, APTR , APTR, UINT32(*)(struct Library *, APTR, struct SysBase* ),UINT32, UINT32)) _GETVECADDR(SysBase, 58))(SysBase,a,b,c,d,e))

#define RomTagScanner(x,y)		(((BOOL(*)(APTR, UINT32 *, UINT32 *))		_GETVECADDR(SysBase, 59))(SysBase,x, y))
#define InitResident(x,y)		(((APTR(*)(APTR, struct Resident *, APTR))		_GETVECADDR(SysBase, 60))(SysBase,x, y))
#define MakeFunctions(x,y,z)		(((void(*)(APTR, APTR, APTR, UINT32))	_GETVECADDR(SysBase, 61))(SysBase,x, y, z))

#define SetTaskPri(x, y)			(((INT8(*)(APTR, APTR, INT16))				_GETVECADDR(SysBase, 62))(SysBase,x, y))

#define SNPrintF(x, y, z)	(((INT32(*)(APTR, char*, UINT32, const char *, ...))	_GETVECADDR(SysBase, 63))(SysBase,x, y, ##z))
#define VPrintF(x, y)		(((void(*)(APTR, const char*, va_list))					_GETVECADDR(SysBase, 64))(SysBase,x, y))
#define VSNPrintF(a, b, c, d)	(((INT32(*)(APTR, char*, UINT32, const char *, va_list))	_GETVECADDR(SysBase, 65))(SysBase,a, b, c, d))


#define MemSet(x, y, z)			(((APTR(*)(APTR, APTR, INT32, UINT32))		_GETVECADDR(SysBase, 66))(SysBase,x, y, z))
#define CopyMem(x, y, z)		(((APTR(*)(APTR, APTR, APTR, INT32))				_GETVECADDR(SysBase, 67))(SysBase,x, y, z))

#define SetSignal(x,y)		(((void(*)(APTR, UINT32, UINT32))						_GETVECADDR(SysBase, 68))(SysBase,x))

#define AddIntServer(x,y)		(((void(*)(APTR, UINT32, struct Interrupt *))					_GETVECADDR(SysBase, 69))(SysBase,x, y))
#define RemIntServer(x,y)		(((struct Interrupt *(*)(APTR, UINT32, struct Interrupt *))		_GETVECADDR(SysBase, 70))(SysBase,x, y))
#define CreateIntServer(a,b,c,d)		(((struct Interrupt *(*)(APTR, const STRPTR, INT8, APTR, APTR))		_GETVECADDR(SysBase, 71))(SysBase,a,b,c,d))

#define AddExcServer(x,y)		(((void(*)(APTR, UINT32, struct Interrupt *))					_GETVECADDR(SysBase, 72))(SysBase,x, y))
#define RemExcServer(x,y)		(((struct Interrupt *(*)(APTR, UINT32, struct Interrupt *))		_GETVECADDR(SysBase, 73))(SysBase,x, y))
#define DPrintF(x...)		(((VOID(*)(APTR, const char *,...))				_GETVECADDR(SysBase, 74))(SysBase, ##x))

#define TaskCreate(a,b,c,d,e)		(((Task*(*)(APTR, const char *, APTR , APTR , UINT32, INT8 ))				_GETVECADDR(SysBase, 75))(SysBase, a,b,c,d,e))

#define InitSemaphore(x)			(((void(*)(APTR, struct SignalSemaphore *))					_GETVECADDR(SysBase, 76))(SysBase,x))
#define AddSemaphore(x,y)			(((void(*)(APTR, const char *, struct SignalSemaphore *))	_GETVECADDR(SysBase, 77))(SysBase,x,y))
#define RemSemaphore(x)				(((void(*)(APTR, struct SignalSemaphore *))					_GETVECADDR(SysBase, 78))(SysBase,x))
#define FindSemaphore(x)			(((struct SignalSemaphore *(*)(APTR, const char *))			_GETVECADDR(SysBase, 79))(SysBase,x))

#define ObtainSemaphore(x)			(((void(*)(APTR, struct SignalSemaphore *))					_GETVECADDR(SysBase, 80))(SysBase,x))
#define AttemptSemaphore(x)			(((void(*)(APTR, struct SignalSemaphore *))					_GETVECADDR(SysBase, 81))(SysBase,x))
#define ReleaseSemaphore(x)			(((void(*)(APTR, struct SignalSemaphore *))					_GETVECADDR(SysBase, 82))(SysBase,x))

