/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     gcc_csky.ld
 * @brief    csky linker file
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
MEMORY
{
ROM(RX)   : ORIGIN = 0x00000000,  LENGTH = 64K
RAM(RWX)  : ORIGIN = 0x20000000,  LENGTH = 4K
}
__kernel_stack = ORIGIN(RAM) + LENGTH(RAM) -8 ;
ENTRY(__start)

SECTIONS {
    .text :
    {
    . = ALIGN(0x4) ;
        *crt0.o (.text)
        *(.text)
		*(.text.*)
    } >ROM

    .RomCode :
    {
    . = ALIGN(0x4) ;
        *(.text)
    } >ROM
	
    .rodata :
    {
    . = ALIGN(0x4) ;
        *(.rodata) 
        *(.rodata.*)
    . = ALIGN(0x4) ; 
        _end_rodata = .;
    } >ROM

    .data : AT(_end_rodata)
    { 
    . = ALIGN(0x4) ;
	_start_data = .;
		KEEP(*csp.o(.data.*))
		KEEP(*csp.o(.data))
    	*( .data );
		*( .data.* );
    . = ALIGN(0x4) ;
    	_end_data = .; 
    } >RAM

    .bss :
    {
         . = ALIGN(0x4) ;
         _bss_start = . ;
             *(.sbss)
             *(.sbss.*)
             *(.scommon)
             *(.bss)
             *(.bss.*)
             *(COMMON)
             . = ALIGN(0x4) ;
          _ebss = . ;
          _end = . ;
          end = . ;
    } >RAM

}

