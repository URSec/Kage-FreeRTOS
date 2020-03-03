/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for STM32L475VGTx Device with
**                1024KByte FLASH, 96KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
**  (c)Copyright Ac6.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Ac6 permit registered System Workbench for MCU users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the System Workbench for MCU toolchain.
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20018000;    /* end of RAM 96k*/

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x0000;     /* required amount of heap */  
_Min_Stack_Size = 0x800; /* 0x758, 0x800 required amount of stack 2K */ /* reduced due to overflow 168 bytes in RAM2 */

/* Specify the memory areas */
MEMORY
{
RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 96K /* 96K --> 0x2001,8000 */
RAM2 (xrw)      : ORIGIN = 0x10000000, LENGTH = 32K  /* 32K --> 0x1000,8000 */
FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 464K  /*1012K, 464K, Use only the first bank */
FLASH_UC (r)	: ORIGIN = 0x08074000, LENGTH = 9K	/* Fixed-location area */
/* FLASH_UC (r)	: ORIGIN = 0x080FD000, LENGTH = 9K  */	/* Fixed-location area */
}

/* Define output sections */
SECTIONS
{ 
  /* uninit region must be last or else it is erased */
  UNINIT_FIXED_LOC (NOLOAD) : ALIGN(0x800)
  {
    *(UNINIT_FIXED_LOC)
  } >FLASH_UC

  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector)) /* Startup code  */
    . = ALIGN(8);
  } >FLASH 
  
  /* Privileged section at the start of the flash - vectors must be first
	whatever. */
  privileged_functions :
  {
  	KEEP(*(.isr_vector))
  	*(privileged_functions)
  } > FLASH
  
  freertos_system_calls :
  {
  	. = ALIGN(8);
  	*(freertos_system_calls)
  	. = ALIGN(8);
  } >FLASH
  

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(8);
    /* Non privileged code kept out of the first 16K of flash. ......fixme.....*/
	/* . = __privileged_functions_start__ + _Privileged_Functions_Region_Size; */
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(8);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >FLASH

  .ARM.extab   : 
  { 
  . = ALIGN(8);
  *(.ARM.extab* .gnu.linkonce.armextab.*)
  . = ALIGN(8);
  } >FLASH
  .ARM : {
	. = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
	. = ALIGN(8);
  } >FLASH

  .preinit_array     :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
	. = ALIGN(8);
  } >FLASH
  
  .init_array :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
	. = ALIGN(8);
  } >FLASH
 
  .fini_array :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
	. = ALIGN(8);
  } >FLASH
  

  /* used by the startup to initialize data */
  /* _sidata = LOADADDR(.data); */
  _sidata = LOADADDR(privileged_data);
 
  privileged_data :
  {
  	. = ALIGN(0x20);
  	__privileged_data_start__ = .;
	_sdata = .;        /* create a global symbol at data start */
  	*(privileged_data);
  	. = ALIGN(0x20);
  	__privileged_data_end__ = .;
  } > RAM2 AT>FLASH

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(8);
    /* _sdata = .;*/   /* sdata is defined in privileged_data section now */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    /* *(.freertos_heap2)*/
    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */
  } >RAM2 AT> FLASH 
  /* >FLASH AT>FLASH */
  /* >RAM2 AT> FLASH */

  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    /* *(.freertos_heap2)*/
    
    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM
  
  .freertos_heap2 :
  {
  	. = ALIGN(4);
  	*(.freertos_heap2);
  	. = ALIGN(4);
  } > RAM AT>FLASH

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM2
  /* >RAM2 */

  

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}

/* define the variables required by FreeRTOS-MPU. First ensure the section sizes
are a binary power of two to comply with the MPU region size rules. */
_Privileged_Functions_Region_Size = 16K; /* 16K , specified in code, port.c: prvSetupMPU() */
/* _Privileged_Data_Region_Size = 256; /* 256 , To-be determined */

/* Then define the variables themselves. */
__FLASH_segment_start__ = ORIGIN( FLASH );
__FLASH_segment_end__ = __FLASH_segment_start__ + LENGTH( FLASH );
__privileged_functions_start__ = ORIGIN( FLASH );
__privileged_functions_end__ = __privileged_functions_start__ + _Privileged_Functions_Region_Size;
__SRAM_segment_start__ = ORIGIN( RAM2 );
/* __SRAM_segment_end__ = __SRAM_segment_start__ + LENGTH( RAM2 ); */
__SRAM_segment_end__ = ORIGIN( RAM ) + LENGTH( RAM ); /* regard RAM as a single region, with a hole in it. */

_Privileged_Data_Region_Size = __privileged_data_end__ - __privileged_data_start__;

/*__privileged_data_start__ = ORIGIN( RAM2 ); */
/*__privileged_data_end__ = ORIGIN( RAM2 ) + _Privileged_Data_Region_Size;*/





