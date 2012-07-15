#ifndef uart_h
#define uart_h

#define PL011_STACK_SIZE	256
#define PL011_PRIORITY		1
#define PL011_QUEUE_SIZE	80

#define UART_BASE	UART0_BASE
#define UART_IRQ	INTERRUPT_UART0
#define UART_CLK	14745600
#define BAUD_RATE	115200

/* UART Registers */
#define UART_DR		(UART_BASE + 0x00)
#define UART_RSR	(UART_BASE + 0x04)
#define UART_ECR	(UART_BASE + 0x04)
#define UART_FR		(UART_BASE + 0x18)
#define UART_IBRD	(UART_BASE + 0x24)
#define UART_FBRD	(UART_BASE + 0x28)
#define UART_LCRH	(UART_BASE + 0x2c)
#define UART_CR		(UART_BASE + 0x30)
#define UART_IMSC	(UART_BASE + 0x38)
#define UART_MIS	(UART_BASE + 0x40)
#define UART_ICR	(UART_BASE + 0x44)


/* Flag register */
#define FR_RXFE		0x10	/* Receive FIFO empty */
#define FR_TXFF		0x20	/* Transmit FIFO full */

/* Masked interrupt status register */
#define MIS_RX		0x10	/* Receive interrupt */
#define MIS_TX		0x20	/* Transmit interrupt */

/* Interrupt clear register */
#define ICR_RX		0x10	/* Clear receive interrupt */
#define ICR_TX		0x20	/* Clear transmit interrupt */

/* Line control register (High) */
#define LCRH_WLEN8	0x60	/* 8 bits */
#define LCRH_FEN	0x10	/* Enable FIFO */

/* Control register */
#define CR_UARTEN	0x0001	/* UART enable */
#define CR_TXE		0x0100	/* Transmit enable */
#define CR_RXE		0x0200	/* Receive enable */

/* Interrupt mask set/clear register */
#define IMSC_RX		0x10	/* Receive interrupt mask */
#define IMSC_TX		0x20	/* Transmit interrupt mask */

#endif
