/* Supports:
 * SDHCI platform device - Arasan SD controller in BCM2708
 *
 */

#include "sdhci_intern.h"
#include "raspi_platform.h"
void lib_DSB(void) ;

#define writel(a,b) ({lib_DSB(); WRITE32(b,a);})
#define writew(a,b) ({lib_DSB(); WRITE16(b,a);})
#define writeb(a,b) ({lib_DSB(); WRITE8(b,a);})

#define readl(a) ({UINT32 __v=READ32(a); lib_DSB();__v;})
#define readw(a) ({UINT16 __v=READ16(a); lib_DSB();__v;})
#define readb(a) ({UINT8 __v=READ8(a); lib_DSB();__v;})

#include "mmc.h"
#include "sdhci.h"


#undef BCM2835_TRACE_REGISTERS

#define	MAX_FREQ	80000000
#define	MIN_FREQ	((MAX_FREQ/1023)/2)

static char *BCMSDH_NAME = "bcm2835_sdh";
static struct sdhci_ops bcm2835_ops;
UINT32 twoticks_delay = 0;
unsigned long long last_write = 0;

#define CONFIG_SYS_HZ			1000000

static UINT32 get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}

UINT32 get_timer(UINT32 base);

inline static void bcm2835_sdhci_raw_writel(struct sdhci_host *host, UINT32 val, INT32 reg)
{

	/* 
	 *The Arasan has a bugette whereby it may lose the content of
	 * successive writes to registers that are within two SD-card clock
   	 * cycles of each other (a clock domain crossing problem).
	 * It seems, however, that the data register does not have this problem.
	 * (Which is just as well - otherwise we'd have to nobble the DMA engine
	 * too)
	 */
 	if (reg != SDHCI_BUFFER && host->clock != 0) {
		UINT32 ns_2clk = 2000000000/host->clock;

		printf("two ticks delay: %d\n", ns_2clk);
		udelay(ns_2clk);
//	while (get_timer(last_write) < 175124) {//twoticks_delay) {
//
//	}
	}
	last_write = get_timer(0);
	writel(val, host->ioaddr + reg);

	/*
	 * Hackish. It seems that some U-Boot timeouts are not 
	 * long enough for the SD card working on default frequency
	 * so until we switch to full speed - create artificial delays
	 */
	if (host->mmc->clock <= MIN_FREQ) {
		udelay(50000);
	}

	if ((reg != SDHCI_BUFFER && reg != SDHCI_INT_STATUS && reg != SDHCI_CLOCK_CONTROL)
	    && host->mmc->clock > MIN_FREQ)
  	{
		INT32 timeout = 100000;
		while (val != readl(host->ioaddr + reg) && --timeout > 0)
		   continue;

		if (timeout <= 0)
			printf("bcm2835_sdhci: writing 0x%X to reg 0x%X "
				   "always gives 0x%X\n",
				   val, reg, readl(host->ioaddr + reg));
	}

}

inline static UINT32 bcm2835_sdhci_raw_readl(struct sdhci_host *host, INT32 reg)
{

	return readl(host->ioaddr + reg);
}

static void bcm2835_sdhci_writel(struct sdhci_host *host, UINT32 val, INT32 reg)
{
#ifdef BCM2835_TRACE_REGISTERS
	printf("SDHCI[%02x] writel %08x\n", reg, val);
#endif
	writel(val, host->ioaddr + reg);
}

static void bcm2835_sdhci_writew(struct sdhci_host *host, UINT16 val, INT32 reg)
{
	static UINT32 shadow = 0;

#ifdef BCM2835_TRACE_REGISTERS
	printf("SDHCI[%02x] writew %04x\n", reg, val);
#endif

	UINT32 p = reg == SDHCI_COMMAND ? shadow :
               bcm2835_sdhci_raw_readl(host, reg & ~3);
	UINT32 s = reg << 3 & 0x18;
	UINT32 l = val << s;
	UINT32 m = 0xffff << s;

	if (reg == SDHCI_TRANSFER_MODE)
		shadow = (p & ~m) | l;
	else {
		bcm2835_sdhci_raw_writel(host, (p & ~m) | l, reg & ~3);
	}
}

static void bcm2835_sdhci_writeb(struct sdhci_host *host, UINT8 val, INT32 reg)
{
#ifdef BCM2835_TRACE_REGISTERS
	printf("SDHCI[%02x] writeb %02x\n", reg, val);
#endif

	UINT32 p = bcm2835_sdhci_raw_readl(host, reg & ~3);
	UINT32 s = reg << 3 & 0x18;
	UINT32 l = val << s;
	UINT32 m = 0xff << s;

	bcm2835_sdhci_raw_writel(host, (p & ~m) | l, reg & ~3);
}

static UINT32 bcm2835_sdhci_readl(struct sdhci_host *host, INT32 reg)
{
	UINT32 val = readl(host->ioaddr + reg);
#ifdef BCM2835_TRACE_REGISTERS
	printf("SDHCI[%02x] readl %08x\n", reg, val);
#endif
	return val;
}

static UINT16 bcm2835_sdhci_readw(struct sdhci_host *host, INT32 reg)
{
	UINT32 val = bcm2835_sdhci_raw_readl(host, (reg & ~3));
	val = val >> (reg << 3 & 0x18) & 0xffff;

#ifdef BCM2835_TRACE_REGISTERS
	printf("SDHCI[%02x] readw %04x\n", reg, val);
#endif

	return (UINT16)val;
}

static UINT8 bcm2835_sdhci_readb(struct sdhci_host *host, INT32 reg)
{
	UINT32 val = bcm2835_sdhci_raw_readl(host, (reg & ~3));
	val = val >> (reg << 3 & 0x18) & 0xff;

#ifdef BCM2835_TRACE_REGISTERS
	printf("SDHCI[%02x] readb %02x\n", reg, val);
#endif

	return (UINT8)val;
}

INT32 bcm2835_sdh_init(UINT32 regbase)
{
	struct sdhci_host *host = NULL;
	UINT32 clock;

	host = (struct sdhci_host *)malloc(sizeof(struct sdhci_host));
	if (!host) {
		printf("sdh_host malloc fail!\n");
		return 1;
	}

	//printf("malloc host: %x\n", host);
	
	host->name = BCMSDH_NAME;
	host->ioaddr = (void *)regbase;
	host->quirks = SDHCI_QUIRK_BROKEN_VOLTAGE | SDHCI_QUIRK_BROKEN_R1B;
	host->voltages = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;;

	memset(&bcm2835_ops, 0, sizeof(struct sdhci_ops));
	bcm2835_ops.write_l = bcm2835_sdhci_writel;
	bcm2835_ops.write_w = bcm2835_sdhci_writew;
	bcm2835_ops.write_b = bcm2835_sdhci_writeb;
	bcm2835_ops.read_l = bcm2835_sdhci_readl;
	bcm2835_ops.read_w = bcm2835_sdhci_readw;
	bcm2835_ops.read_b = bcm2835_sdhci_readb;
	host->ops = &bcm2835_ops;

	/*
	 * Convert SD controller ticks to CPU ticks
	 */
	twoticks_delay = ((2000000000ULL/MIN_FREQ) + 1)*get_tbclk()/1000000;
	twoticks_delay = (twoticks_delay + 1000 - 1) / 1000;

	host->version = sdhci_readw(host, SDHCI_HOST_VERSION) & 0xff;
	//printf("version: %d\n", host->version);
	add_sdhci(host, MIN_FREQ, 0);

	return 0;
}

INT32 board_mmc_init()
{

	bcm2835_sdh_init(EMMC_BASE);
}
