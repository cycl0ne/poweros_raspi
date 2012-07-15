#ifndef sdhci_h
#define sdhci_h

#include "types.h"
#include "device.h"
#include "timer.h"
#include "io.h"

#undef SysBase




#include "block_device.h"

#define VERSION  0
#define REVISION 1

#undef DEVNAME
#define DEVNAME "sdhci"

struct IOExtTD
{
    struct IOStdReq iotd_Req;
    UINT32           iotd_Count;
    UINT32           iotd_SecLabel;
};

#define TD_MOTOR        (CMD_NONSTD + 0)
#define TD_SEEK         (CMD_NONSTD + 1)
#define TD_FORMAT       (CMD_NONSTD + 2)
#define TD_REMOVE       (CMD_NONSTD + 3)
#define TD_CHANGENUM    (CMD_NONSTD + 4)
#define TD_CHANGESTATE  (CMD_NONSTD + 5)
#define TD_PROTSTATUS   (CMD_NONSTD + 6)
#define TD_RAWREAD      (CMD_NONSTD + 7)
#define TD_RAWWRITE     (CMD_NONSTD + 8)
#define TD_GETDRIVETYPE (CMD_NONSTD + 9) /* see below */
#define TD_GETNUMTRACKS (CMD_NONSTD + 10)
#define TD_ADDCHANGEINT (CMD_NONSTD + 11)
#define TD_REMCHANGEINT (CMD_NONSTD + 12)
#define TD_GETGEOMETRY  (CMD_NONSTD + 13) /* returns (DriveGeometry *) */
#define TD_EJECT        (CMD_NONSTD + 14)
#define TD_LASTCOMM     (CMD_NONSTD + 15)

struct DriveGeometry
{
    UINT32 dg_SectorSize;
    UINT32 dg_TotalSectors;
    UINT32 dg_Cylinders;
    UINT32 dg_CylSectors;
    UINT32 dg_Heads;
    UINT32 dg_TrackSectors;
    UINT32 dg_BufMemType;
    UINT8 dg_DeviceType;   /* see below */
    UINT8 dg_Flags;        /* see below */
    UINT16 dg_Reserved;
};

/* dg_DeviceType */
#define DG_DIRECT_ACCESS     0
#define DG_SEQUENTIAL_ACCESS 1
#define DG_PRINTER           2
#define DG_PROCESSOR         3
#define DG_WORM              4
#define DG_CDROM             5
#define DG_SCANNER           6
#define DG_OPTICAL_DISK      7
#define DG_MEDIUM_CHANGER    8
#define DG_COMMUNICATION     9
#define DG_UNKNOWN           31

/* dg_Flags */
#define DGB_REMOVABLE     0
#define DGF_REMOVABLE (1<<0)


#define TDERR_NotSpecified   20
#define TDERR_NoSecHdr       21
#define TDERR_BadSecPreamble 22
#define TDERR_BadSecID       23
#define TDERR_BadHdrSum      24
#define TDERR_BadSecSum      25
#define TDERR_TooFewSecs     26
#define TDERR_BadSecHdr      27
#define TDERR_WriteProt      28
#define TDERR_DiskChanged    29
#define TDERR_SeekError      30
#define TDERR_NoMem          31
#define TDERR_BadUnitNum     32
#define TDERR_BadDriveType   33
#define TDERR_DriveInUse     34
#define TDERR_PostReset      35

typedef struct SDHCIFlags {
	union {
	UINT32	RESERVED:4;
	UINT32	DELEXP	:1;
	UINT32	PAULA	:1;
	UINT32	WRITING	:1;
	UINT32	RESET	:1;
	UINT32	RESERVED2:24;
	} bits;
}SDHCIFlags;

#define SDHCI_SIGNAL_DEVDONE	8;
#define SDHCI_SIGNAL_PORT		9;
#define SDHCI_SIGNAL_WAITTIMER	10;
#define SDHCI_SIGNAL_DSKBLK		11;
/*
typedef struct SDHCIUnit
{
	UINT32	Flags;
	UINT32	DeviceType;
	UINT32	NumberOfTracks;
	char name[32];
	void *priv;
	UINT32 voltages;
	UINT32 version;
	UINT32 has_init;
	UINT32 f_min;
	UINT32 f_max;
	int high_capacity;
	UINT32 bus_width;
	UINT32 clock;
	UINT32 card_caps;
	UINT32 host_caps;
	UINT32 ocr;
	UINT32 scr[2];
	UINT32 csd[4];
	UINT32 cid[4];
	UINT16 rca;
	char part_config;
	char part_num;
	UINT32 tran_speed;
	UINT32 read_bl_len;
	UINT32 write_bl_len;
	UINT32 erase_grp_size;
	UINT64 capacity;
	block_dev_desc_t	block_dev;
	int (*send_cmd)(struct SDHCIUnit *mmc, struct mmc_cmd *cmd, struct mmc_data *data);
	void (*set_ios)(struct SDHCIUnit *mmc);
	int (*init)(struct SDHCIUnit *mmc);
	int (*getcd)(struct SDHCIUnit *mmc);
	UINT32	b_max;
} SDHCIUnit;
*/
typedef struct SDHCIBase
{
	struct Device		Device;
	UINT32				Flags;
	APTR				SysBase;
	//struct SDHCIUnit	Units; // Support only one Unit
	
	struct Task			*SDHCITask;
	APTR				Timer0_Dev;
    struct TimeRequest  Timer0_Unit;
	APTR				Timer1_Dev;
    struct TimeRequest  Timer1_Unit;

	UINT32				DMAIRQ;
	struct Interrupt	*SDHCI_IntServer;
} SDHCIBase;

#define size_t UINT32

void printf(const char *fmt, ...);
APTR malloc(UINT32 byteSize);
void free(APTR mem);
void *memcpy(void* dest, void* src, int n);
void memset(void* m, int c, UINT32 n);
void udelay(UINT32 delay);
void mdelay(UINT32 delay);
int memcmp(const void *s1, const void *s2, size_t n);
void sprintf(char *buf, const char *fmt, ...);
#define bd_t UINT32

#define SysBase SDHCIBase->SysBase

#endif
