#include "types.h"
#include "sysbase.h"
#include "exec_funcs.h"

#define GPFSEL1     0x20200004
#define GPPUD       0x20200094
#define GPPUDCLK0   0x20200098

#define AUX_ENABLES     0x20215004
#define AUX_MU_IO_REG   0x20215040
#define AUX_MU_IER_REG  0x20215044
#define AUX_MU_IIR_REG  0x20215048
#define AUX_MU_LCR_REG  0x2021504C
#define AUX_MU_MCR_REG  0x20215050
#define AUX_MU_LSR_REG  0x20215054
#define AUX_MU_MSR_REG  0x20215058
#define AUX_MU_SCRATCH  0x2021505C
#define AUX_MU_CNTL_REG 0x20215060
#define AUX_MU_STAT_REG 0x20215064
#define AUX_MU_BAUD_REG 0x20215068

extern void dummy ( unsigned int );

INT32 lib_RawMayGetChar(struct SysBase *SysBase)
{
	if(!READ32(AUX_MU_LSR_REG)&0x01) return -1;
	return(READ32(AUX_MU_IO_REG)&0xFF);
}

void lib_RawPutChar(struct SysBase *SysBase, UINT8 chr)
{
	UINT32 c = chr;
	if ((c&0x000000ff) == '\n')
	{
		while(1) {if(READ32(AUX_MU_LSR_REG)&0x20) break;}
		WRITE32(AUX_MU_IO_REG,0x0d);
		while(1) {if(READ32(AUX_MU_LSR_REG)&0x20) break;}
		WRITE32(AUX_MU_IO_REG,0x0a);
	
	} else 
	{	
		while(1) {if(READ32(AUX_MU_LSR_REG)&0x20) break;}
		WRITE32(AUX_MU_IO_REG,c);
	}
}

void lib_RawIOInit(struct SysBase *SysBase)
{
    unsigned int ra;
    WRITE32(AUX_ENABLES,1);
    WRITE32(AUX_MU_IER_REG,0);
    WRITE32(AUX_MU_CNTL_REG,0);
    WRITE32(AUX_MU_LCR_REG,3);
    WRITE32(AUX_MU_MCR_REG,0);
    WRITE32(AUX_MU_IER_REG,0);
    WRITE32(AUX_MU_IIR_REG,0xC6);
    WRITE32(AUX_MU_BAUD_REG,270);
    ra=READ32(GPFSEL1);
    ra&=~(7<<12); //gpio14
    ra|=2<<12;    //alt5
    ra&=~(7<<15); //gpio15
    ra|=2<<15;    //alt5
    WRITE32(GPFSEL1,ra);
    WRITE32(GPPUD,0);
    for(ra=0;ra<150;ra++) dummy(ra);
    WRITE32(GPPUDCLK0,(1<<14)|(1<<15));
    for(ra=0;ra<150;ra++) dummy(ra);
    WRITE32(GPPUDCLK0,0);
    WRITE32(AUX_MU_CNTL_REG,3);
}