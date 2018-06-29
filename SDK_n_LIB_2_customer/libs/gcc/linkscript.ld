MEMORY
{
	ROM (rx) :  ORIGIN = 0x08292040, LENGTH = 0x00032000
	RAM (rwx) : ORIGIN = 0x001E7000, LENGTH = 0x00019000
}

SECTIONS
{
    . = 0x08292040;
	.initdata :
	{ 
	. = ALIGN(4);
		KEEP(*(.initdata));
	}AT > ROM	
	text : 
	{ 
	. = ALIGN(4);
	    *(.text.*)
	    *(.text) 
	}AT > ROM
	.rodata :
	{
	. = ALIGN(4);
	    *(.rodata)
	}AT > ROM
	__data_load = LOADADDR(.data);
	.data :
	{
		__data_start = .;
		. = ALIGN(4);
		*(.data.*)
		*(.data)
		_edata = .;
	} > RAM  AT > ROM
	.bss : 
	{
		__bss_start__ = .;
		. = ALIGN(4);
		*(.bss)
		*(.bss.*)
		__bss_end__ = .;
	} > RAM
}