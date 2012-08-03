#include "types.h"
#include "raspi_platform.h"
#include "raspi_irq.h"
#include "uart.h"

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

unsigned int lib_UART_lcr ( void );
unsigned int lib_UART_recv ( void );
void lib_UART_send ( unsigned int c );

extern void dummy ( unsigned int );

void lib_InitUART();


void lib_ROMWackInit()
{
	lib_InitUART();
	//irq_enable(INTERRUPT_UART0, &lib_UART_IRQ);
}

void lib_Print_uart0(const char *s) 
{
	while(*s != '\0') { /* Loop until end of string */
	if (*s == '\n') 
	{
		lib_UART_send(0x0D);
		lib_UART_send(0x0A);
	} else
		lib_UART_send(*s); /* Transmit char */
	s++; /* Next char */
	}
}

void lib_hexstrings ( unsigned int d )
{
    //unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        lib_UART_send(rc);
        if(rb==0) break;
    }
    lib_UART_send(0x20);
    lib_UART_send(0x0D);
    lib_UART_send(0x0A);
}

unsigned int lib_UART_lcr ( void )
{
    return(READ32(AUX_MU_LSR_REG));
}

unsigned int lib_UART_recv ( void )
{
    while(1)
    {
        if(READ32(AUX_MU_LSR_REG)&0x01) break;
    }
    return(READ32(AUX_MU_IO_REG)&0xFF);
}
void lib_DSB(void);

void lib_UART_send ( unsigned int c )
{
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
		lib_DSB();
	}
}

// Not Used at the moment
/*
void lib_UART_IRQ(void) 
{
	UINT32 mis;

	mis = READ32(UART_MIS);

	if (mis & MIS_RX) {
		// Clear interrupt status 
		WRITE32(UART_ICR, ICR_RX);
		
		while (READ32(UART_FR) & FR_RXFE) DMB;
		do {
			*__pl011_in_write_ptr++ = (UINT8)READ32(UART_DR);
			if ((__pl011_in_write_ptr - __pl011_in_read_buffer) > PL011_BUFFER_SIZE) __pl011_in_write_ptr = __pl011_in_read_buffer;
		} while ((READ32(UART_FR) & FR_RXFE) == 0);
	}	
}
*/

void lib_InitUART( void )
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
