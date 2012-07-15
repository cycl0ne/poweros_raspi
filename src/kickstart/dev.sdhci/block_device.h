#ifndef block_device
#define block_device
#define lbaint_t UINT64

typedef struct block_dev_desc {
	int		if_type;	/* type of the interface */
	int		dev;		/* device number */
	unsigned char	part_type;	/* partition type */
	unsigned char	target;		/* target SCSI ID */
	unsigned char	lun;		/* target LUN */
	unsigned char	type;		/* device type */
	unsigned char	removable;	/* removable device */
	unsigned char	lba48;		/* device can use 48bit addr (ATA/ATAPI v7) */
	unsigned long	lba;		/* number of blocks */
	unsigned long	blksz;		/* block size */
	char			vendor [40+1];	/* IDE model, SCSI Vendor */
	char			product[20+1];	/* IDE Serial no, SCSI product */
	char			revision[8+1];	/* firmware revision */

	UINT32	(*block_read)(INT32 dev,
				      UINT32 start,
				      lbaint_t blkcnt,
				      void *buffer);
	UINT32	(*block_write)(INT32 dev,
				       UINT32 start,
				       lbaint_t blkcnt,
				       const void *buffer);
	UINT32   (*block_erase)(INT32 dev,
				       UINT32 start,
				       lbaint_t blkcnt);

	void		*priv;		/* driver private struct pointer */
}block_dev_desc_t;

/* Interface types: */
#define IF_TYPE_UNKNOWN		0
#define IF_TYPE_IDE		1
#define IF_TYPE_SCSI		2
#define IF_TYPE_ATAPI		3
#define IF_TYPE_USB		4
#define IF_TYPE_DOC		5
#define IF_TYPE_MMC		6
#define IF_TYPE_SD		7
#define IF_TYPE_SATA		8

/* Part types */
#define PART_TYPE_UNKNOWN	0x00
#define PART_TYPE_MAC		0x01
#define PART_TYPE_DOS		0x02
#define PART_TYPE_ISO		0x03
#define PART_TYPE_AMIGA		0x04
#define PART_TYPE_EFI		0x05

/*
 * Type string for U-Boot bootable partitions
 */
#define BOOT_PART_TYPE	"U-Boot"	/* primary boot partition type	*/
#define BOOT_PART_COMP	"PPCBoot"	/* PPCBoot compatibility type	*/

/* device types */
#define DEV_TYPE_UNKNOWN	0xff	/* not connected */
#define DEV_TYPE_HARDDISK	0x00	/* harddisk */
#define DEV_TYPE_TAPE		0x01	/* Tape */
#define DEV_TYPE_CDROM		0x05	/* CD-ROM */
#define DEV_TYPE_OPDISK		0x07	/* optical disk */

typedef struct disk_partition {
	UINT32	start;		/* # of first block in partition	*/
	UINT32	size;		/* number of blocks in partition	*/
	UINT32	blksz;		/* block size in bytes			*/
	UINT8	name[32];	/* partition name			*/
	UINT8	type[32];	/* string type description		*/
} disk_partition_t;

#endif
