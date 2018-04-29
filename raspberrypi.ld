/**
 * BlueThunder Linker Script for the raspberry Pi!
 *
 *
 *
 **/
MEMORY
{
	RESERVED	(r)		: ORIGIN = 0x00000000, LENGTH = 32K
	INIT_RAM	(rwx)	: ORIGIN = 0x00008000, LENGTH = 32K
	RAM			(rwx)	: ORIGIN = 0x00010000, LENGTH = 128M
	HEAP		(rwx)	: ORIGIN = 0x08010000, LENGTH = 64M
}

ENTRY(_start)

SECTIONS {
    /*
	 * Our init section allows us to place the bootstrap code at address 0x8000
	 *
	 * This is where the Graphics processor forces the ARM to start execution.
	 * However the interrupt vector code remains at 0x0000, and so we must copy the correct
	 * branch instructions to 0x0000 - 0x001C in order to get the processor to handle interrupts.
	 *
	 */
	.init : {
		KEEP(*(.init))
	} > INIT_RAM = 0

	.module_entries : {
		__module_entries_start = .;
	  	KEEP(*(.module_entries))
		KEEP(*(.module_entries.*))
		__module_entries_end = .;
		__module_entries_size = SIZEOF(.module_entries);
	} > INIT_RAM


	/**
 	 *	This is the main code section, it is essentially of unlimited size. (128Mb).
	 *
	 **/
	.text : {
		*(.text)
	} > RAM

	/*
	* Next we put the data.
	*/
	.data : {
		*(.data)
	} > RAM

	.bss :
	{
		__bss_start = .;
		__bss_start__ = __bss_start;
		*(.bss)
		*(.bss.*)
		__bss_end = .;
		__bss_end__ = __bss_end;
	} > RAM

	/**
	 *	Place HEAP here???
	 **/
    .heap : {
        __heap_start = .;
		end = __heap_start;
		_end = end;
		__end = end;
        *(.heap)
        __heap_end = ORIGIN(HEAP) + LENGTH(HEAP);
    } > HEAP
    /*PROVIDE(__HEAP_START = __bss_end );*/
    
	/**
	 *	Stack starts at the top of the RAM, and moves down!
	 * _estack = ORIGIN(RAM) + LENGTH(RAM);
	 **/
	_estack = ORIGIN(HEAP) + LENGTH(HEAP);
}

