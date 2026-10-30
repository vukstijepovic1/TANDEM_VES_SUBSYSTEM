/* Linker script to configure memory regions. */

MEMORY
{
	LOADER (rx)			: ORIGIN = 0x00000000, LENGTH = 28K
	SETTINGS (rw)		: ORIGIN = 0x00007000, LENGTH = 4K
	RESERVED (rw)		: ORIGIN = 0x00008000, LENGTH = 32K
	PROGRAM (rwx)		: ORIGIN = 0x00010000, LENGTH = 224K
	UPDATE (rwx)		: ORIGIN = 0x00048000, LENGTH = 224K

	RAM (rwx)			: ORIGIN = 0x10000000, LENGTH = 32K

	USB_RAM(rwx)		: ORIGIN = 0x2007C000, LENGTH = 16K
	ETH_RAM(rwx)		: ORIGIN = 0x20080000, LENGTH = 16K
}

ENTRY(Reset_Handler)

SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		*(.text*)

		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)

		*(.rodata*)

		KEEP(*(.eh_frame*))
	} > PROGRAM

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > PROGRAM

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > PROGRAM
	__exidx_end = .;

	__etext = .;

	.data : AT (__etext)
	{
		__data_start__ = .;
		*(vtable)
		*(.data*)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE (__init_array_end = .);


		. = ALIGN(4);
		/* finit data */
		PROVIDE (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE (__fini_array_end = .);

		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;

	} > RAM

	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > RAM


	.heap :
	{
	    __HeapBase = .;
		__end__ = .;
		end = __end__;
		*(.heap*)
		__HeapLimit = .;
	} > RAM

	/* .stack_dummy section doesn't contains any symbols. It is only   *
	 * used for linker to calculate size of stack sections, and assign *
	 * values to stack symbols later                                   */
	.stack_dummy :
	{
		*(.stack)
	} > RAM

	/* Set stack top to end of RAM, and stack limit move down by *
	 * size of stack_dummy section                               */
	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")


	/* Code can explicitly ask for data to be *
	 * placed in these higher RAM banks where *
	 * they will be left uninitialized.       */
	.AHBSRAM0 (NOLOAD):
	{
		*(AHBSRAM0)
	} > USB_RAM

	.AHBSRAM1 (NOLOAD):
	{
		*(AHBSRAM1)
	} > ETH_RAM

}
