#ifdef DEVCONF
	#include "devconf.h"
#endif

#ifndef STACK_SIZE
	#define STACK_SIZE 0x400
#endif
#ifndef HEAP_SIZE
	#define HEAP_SIZE 0
#endif

	.syntax unified
	.arch armv7-m

/* Memory Model                                                             *
 * The HEAP starts at the end of the DATA section and grows upward.         *
 * The STACK starts at the end of the RAM and grows downward.               *
 * The HEAP and stack STACK are only checked at compile time:               *
 * (DATA_SIZE + HEAP_SIZE + STACK_SIZE) < RAM_SIZE                          *
 *                                                                          *
 * This is just a check for the bare minimum for the Heap+Stack area before *
 * aborting compilation, it is not the run time limit.                      */

	.section .stack
	.align 3
	.equ    Stack_Size, STACK_SIZE
	.globl    __StackTop
	.globl    __StackLimit
__StackLimit:
	.space    Stack_Size
	.size __StackLimit, . - __StackLimit
__StackTop:
	.size __StackTop, . - __StackTop

	.section .heap
	.align 3
	.equ    Heap_Size, HEAP_SIZE
	.globl    __HeapBase
	.globl    __HeapLimit
__HeapBase:
	.space    Heap_Size
	.size __HeapBase, . - __HeapBase
__HeapLimit:
	.size __HeapLimit, . - __HeapLimit

	.section .isr_vector
	.align 2
	.globl __isr_vector
__isr_vector:
	.word    __StackTop            /* Top of Stack */
	.word    Reset_Handler         /* Reset Handler */
	.word    NMI_Handler           /* NMI Handler */
	.word    HardFault_Handler     /* Hard Fault Handler */
	.word    MemManage_Handler     /* MPU Fault Handler */
	.word    BusFault_Handler      /* Bus Fault Handler */
	.word    UsageFault_Handler    /* Usage Fault Handler */
	.word    0                     /* Reserved */
	.word    0                     /* Reserved */
	.word    0                     /* Reserved */
    .word    0                     /* Reserved */
	.word    SVC_Handler           /* SVCall Handler */
	.word    DebugMon_Handler      /* Debug Monitor Handler */
	.word    0                     /* Reserved */
	.word    PendSV_Handler        /* PendSV Handler */
	.word    SysTick_Handler       /* SysTick Handler */

	/* External interrupts */
	.word   WDT_IRQHandler              /* 16: Watchdog Timer               */
	.word   TIMER0_IRQHandler           /* 17: Timer0                       */
	.word   TIMER1_IRQHandler           /* 18: Timer1                       */
	.word   TIMER2_IRQHandler           /* 19: Timer2                       */
	.word   TIMER3_IRQHandler           /* 20: Timer3                       */
	.word   UART0_IRQHandler            /* 21: UART0                        */
	.word   UART1_IRQHandler            /* 22: UART1                        */
	.word   UART2_IRQHandler            /* 23: UART2                        */
	.word   UART3_IRQHandler            /* 24: UART3                        */
	.word   PWM1_IRQHandler             /* 25: PWM1                         */
	.word   I2C0_IRQHandler             /* 26: I2C0                         */
	.word   I2C1_IRQHandler             /* 27: I2C1                         */
	.word   I2C2_IRQHandler             /* 28: I2C2                         */
	.word   SPI_IRQHandler              /* 29: SPI                          */
	.word   SSP0_IRQHandler             /* 30: SSP0                         */
	.word   SSP1_IRQHandler             /* 31: SSP1                         */
	.word   PLL0_IRQHandler             /* 32: PLL0 Lock (Main PLL)         */
	.word   RTC_IRQHandler              /* 33: Real Time Clock              */
	.word   EINT0_IRQHandler            /* 34: External Interrupt 0         */
	.word   EINT1_IRQHandler            /* 35: External Interrupt 1         */
	.word   EINT2_IRQHandler            /* 36: External Interrupt 2         */
	.word   EINT3_IRQHandler            /* 37: External Interrupt 3         */
	.word   ADC_IRQHandler              /* 38: A/D Converter                */
	.word   BOD_IRQHandler              /* 39: Brown-Out Detect             */
	.word   USB_IRQHandler              /* 40: USB                          */
	.word   CAN_IRQHandler              /* 41: CAN                          */
	.word   DMA_IRQHandler              /* 42: General Purpose DMA          */
	.word   I2S_IRQHandler              /* 43: I2S                          */
	.word   ENET_IRQHandler             /* 44: Ethernet                     */
	.word   RIT_IRQHandler              /* 45: Repetitive Interrupt Timer   */
	.word   MCPWM_IRQHandler            /* 46: Motor Control PWM            */
	.word   QEI_IRQHandler              /* 47: Quadrature Encoder Interface */
	.word   PLL1_IRQHandler             /* 48: PLL1 Lock (USB PLL)          */
	.word   USBActivity_IRQHandler      /* 49: USB Activity                 */
	.word   CANActivity_IRQHandler      /* 50: CAN Activity                 */

	.size    __isr_vector, . - __isr_vector

	.text
	.thumb
	.thumb_func
	.align 2
	.globl    Reset_Handler
	.type    Reset_Handler, %function
Reset_Handler:
/* Loop to zero initialize data in BSS section inside RAM. The ranges of           *
 * copy from/to are specified by following symbols evaluated in the linker script. *
 * __bss_start__: Beginning of the bss section                                     *
 * __bss_end__: End of the bss section                                             */

	ldr    r0, =0x000000
	ldr    r1, =__bss_start__
	ldr    r2, =__bss_end__

.Lfill_bss_loop:
 	cmp    r1, r2
	itt    lt
	strlt   r0, [r1], #4
	blt    .Lfill_bss_loop

/* Loop to copy data from read only memory to RAM. The ranges              *
 * of copy from/to are specified by following symbols evaluated in         *
 * linker script.                                                          *
 * _etext: End of code section, i.e., begin of data sections to copy from. *
 * __data_start__/__data_end__: RAM address range that data should be      *
 * copied to. Both must be aligned to 4 bytes boundary.                    */

	ldr    r1, =__etext
	ldr    r2, =__data_start__
 	ldr    r3, =__data_end__

.Lflash_to_ram_loop:
	cmp     r2, r3
	ittt    lt
	ldrlt   r0, [r1], #4
	strlt   r0, [r2], #4
	blt    .Lflash_to_ram_loop

/* Call static constructors */
	bl     __libc_init_array
/* Go to main */
	b      main
	.pool
	.size Reset_Handler, . - Reset_Handler

	.text
/* Macro to define default handlers. Default handler    *
 * will be weak symbol and just dead loops. They can be *
 * overwritten by other handlers                        */
	.macro    def_default_handler    handler_name
	.align 1
	.thumb_func
	.weak    \handler_name
	.type    \handler_name, %function
\handler_name :
	b    .
	.size    \handler_name, . - \handler_name
	.endm

	def_default_handler    NMI_Handler
	def_default_handler    HardFault_Handler
	def_default_handler    MemManage_Handler
	def_default_handler    BusFault_Handler
	def_default_handler    UsageFault_Handler
	def_default_handler    SVC_Handler
	def_default_handler    DebugMon_Handler
	def_default_handler    PendSV_Handler
	def_default_handler    SysTick_Handler
	def_default_handler    Default_Handler

	.macro    def_irq_default_handler    handler_name
	.weak     \handler_name
	.thumb_set      \handler_name, Default_Handler
	.endm

	def_irq_default_handler     WDT_IRQHandler
	def_irq_default_handler     TIMER0_IRQHandler
	def_irq_default_handler     TIMER1_IRQHandler
	def_irq_default_handler     TIMER2_IRQHandler
	def_irq_default_handler     TIMER3_IRQHandler
	def_irq_default_handler     UART0_IRQHandler
	def_irq_default_handler     UART1_IRQHandler
	def_irq_default_handler     UART2_IRQHandler
	def_irq_default_handler     UART3_IRQHandler
	def_irq_default_handler     PWM1_IRQHandler
	def_irq_default_handler     I2C0_IRQHandler
	def_irq_default_handler     I2C1_IRQHandler
	def_irq_default_handler     I2C2_IRQHandler
	def_irq_default_handler     SPI_IRQHandler
	def_irq_default_handler     SSP0_IRQHandler
	def_irq_default_handler     SSP1_IRQHandler
	def_irq_default_handler     PLL0_IRQHandler
	def_irq_default_handler     RTC_IRQHandler
	def_irq_default_handler     EINT0_IRQHandler
	def_irq_default_handler     EINT1_IRQHandler
	def_irq_default_handler     EINT2_IRQHandler
	def_irq_default_handler     EINT3_IRQHandler
	def_irq_default_handler     ADC_IRQHandler
	def_irq_default_handler     BOD_IRQHandler
	def_irq_default_handler     USB_IRQHandler
	def_irq_default_handler     CAN_IRQHandler
	def_irq_default_handler     DMA_IRQHandler
	def_irq_default_handler     I2S_IRQHandler
	def_irq_default_handler     ENET_IRQHandler
	def_irq_default_handler     RIT_IRQHandler
	def_irq_default_handler     MCPWM_IRQHandler
	def_irq_default_handler     QEI_IRQHandler
	def_irq_default_handler     PLL1_IRQHandler
	def_irq_default_handler     USBActivity_IRQHandler
	def_irq_default_handler     CANActivity_IRQHandler
	def_irq_default_handler     DEF_IRQHandler

	.end
