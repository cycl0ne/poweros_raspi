#ifndef RASPI_PLATFORM_H
#define RASPI_PLATFORM_H

#define IO_BASE_CPU   0x20000000
#define IO_BASE_GPU   0x7e000000

#define SYSTIMER_OFFSET 0x00003000
#define SYSTIMER_BASE   IO_BASE_CPU + SYSTIMER_OFFSET

#define IRQ_OFFSET      0x0000b200
#define IRQ_BASE        IO_BASE_CPU + IRQ_OFFSET
#define ARM_IRQ_PEND3   IRQ_BASE + 0x00
#define ARM_IRQ_PEND1   IRQ_BASE + 0x04
#define ARM_IRQ_PEND2   IRQ_BASE + 0x08
#define ARM_FIQ_CTRL    IRQ_BASE + 0x0c
#define ARM_IRQ_ENBL1   IRQ_BASE + 0x10 /* Bank1 enable bits */
#define ARM_IRQ_ENBL2   IRQ_BASE + 0x14 /* Bank2 enable bits */
#define ARM_IRQ_ENBL3   IRQ_BASE + 0x18 /* ARM irqs enable bits */
#define ARM_IRQ_DIBL1   IRQ_BASE + 0x1C /* Bank1 disable bits */
#define ARM_IRQ_DIBL2   IRQ_BASE + 0x20 /* Bank2 disable bits */
#define ARM_IRQ_DIBL3   IRQ_BASE + 0x24 /* ARM irqs disable bits */

#define HW_REGISTER_RW(addr) (addr)
#define HW_REGISTER_RO(addr) (addr)

/* Timer */
/* For reg. fields see sp804 spec. */
#define ARM_T_LOAD    HW_REGISTER_RW(ARM_BASE+0x400)
#define ARM_T_VALUE   HW_REGISTER_RW(ARM_BASE+0x404)
#define ARM_T_CONTROL HW_REGISTER_RW(ARM_BASE+0x408)
#define ARM_T_IRQCNTL HW_REGISTER_RW(ARM_BASE+0x40C)
#define ARM_T_RAWIRQ  HW_REGISTER_RW(ARM_BASE+0x410)
#define ARM_T_MSKIRQ  HW_REGISTER_RW(ARM_BASE+0x414)
#define ARM_T_RELOAD  HW_REGISTER_RW(ARM_BASE+0x418)
#define ARM_T_PREDIV  HW_REGISTER_RW(ARM_BASE+0x41c)
#define ARM_T_FREECNT HW_REGISTER_RW(ARM_BASE+0x420)

#define TIMER_CTRL_ONESHOT  (1 << 0)
#define TIMER_CTRL_32BIT    (1 << 1)
#define TIMER_CTRL_DIV1     (0 << 2)
#define TIMER_CTRL_DIV16    (1 << 2)
#define TIMER_CTRL_DIV256   (2 << 2)
#define TIMER_CTRL_IE       (1 << 5)
#define TIMER_CTRL_PERIODIC (1 << 6)
#define TIMER_CTRL_ENABLE   (1 << 7)
#define TIMER_CTRL_DBGHALT  (1 << 8)
#define TIMER_CTRL_ENAFREE  (1 << 9)
#define TIMER_CTRL_FREEDIV_SHIFT 16)
#define TIMER_CTRL_FREEDIV_MASK  0xff

#define ARMTIMER_OFFSET  0x0000b400
#define ARMTIMER_BASE    IO_BASE_CPU + ARMTIMER_OFFSET
#define ARMTIMER_LOAD    ARMTIMER_BASE + 0x00
#define ARMTIMER_VALUE   ARMTIMER_BASE + 0x04
#define ARMTIMER_CONTROL ARMTIMER_BASE + 0x08
#define ARMTIMER_IRQ_ACK ARMTIMER_BASE + 0x0c
#define ARMTIMER_IRQ_RAW ARMTIMER_BASE + 0x10
#define ARMTIMER_IRQ_MSK ARMTIMER_BASE + 0x14
#define ARMTIMER_RELOAD  ARMTIMER_BASE + 0x18
#define ARMTIMER_PREDIV  ARMTIMER_BASE + 0x1c
#define ARMTIMER_FRC     ARMTIMER_BASE + 0x20


#define SYSTICKS_HZ  1000
#define CLOCK_RATE    250000000
#define TIMER_COUNT   (CLOCK_RATE / SYSTICKS_HZ)

// From Linux
#define BCM2708_PERI_BASE        0x20000000
#define ST_BASE                  (BCM2708_PERI_BASE + 0x3000)   /* System Timer */
#define DMA_BASE		 (BCM2708_PERI_BASE + 0x7000)	/* DMA controller */
#define ARM_BASE                 (BCM2708_PERI_BASE + 0xB000)	 /* BCM2708 ARM control block */
#define PM_BASE			 (BCM2708_PERI_BASE + 0x100000) /* Power Management, Reset controller and Watchdog registers */
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO */
#define UART0_BASE               (BCM2708_PERI_BASE + 0x201000)	/* Uart 0 */
#define MMCI0_BASE               (BCM2708_PERI_BASE + 0x202000) /* MMC interface */
#define UART1_BASE               (BCM2708_PERI_BASE + 0x215000) /* Uart 1 */
#define EMMC_BASE                (BCM2708_PERI_BASE + 0x300000) /* eMMC interface */
#define SMI_BASE		 (BCM2708_PERI_BASE + 0x600000) /* SMI */
#define USB_BASE                 (BCM2708_PERI_BASE + 0x980000) /* DTC_OTG USB controller */
#define MCORE_BASE               (BCM2708_PERI_BASE + 0x0000)   /* Fake frame buffer device (actually the multicore sync block*/

#define ARMCTRL_BASE             (ARM_BASE + 0x000)
#define ARMCTRL_IC_BASE          (ARM_BASE + 0x200)           /* ARM interrupt controller */
#define ARMCTRL_TIMER0_1_BASE    (ARM_BASE + 0x400)           /* Timer 0 and 1 */
#define ARMCTRL_0_SBM_BASE       (ARM_BASE + 0x800)           /* User 0 (ARM)'s Semaphores Doorbells and Mailboxes */

#define GPFSEL1     GPIO_BASE + 0x04
#define GPSET0      GPIO_BASE + 0x1c
#define GPCLR0      GPIO_BASE + 0x28
#define GPLEV0      GPIO_BASE + 0x34

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define DMB_IMPL(name) { UINT32 name = 0; __asm__ __volatile__ ("mcr	p15,0,%[t],c7,c10,5\n" :: [t] "r" (name) : "memory"); }
#define DSB_IMPL(name) { UINT32 name = 0; __asm__ __volatile__ ("mcr	p15,0,%[t],c7,c10,4\n" :: [t] "r" (name) : "memory"); }

#define DMB DMB_IMPL(MACRO_CONCAT(__dmb_, __COUNTER__))
#define DSB DSB_IMPL(MACRO_CONCAT(__dsb_, __COUNTER__))

#endif
