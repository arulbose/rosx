#include "defines.h"

MEMORY
{
    ram : ORIGIN = 0x8000, LENGTH = 0x2000000
}

SECTIONS
{
   .text : {
    out/arch/arm/cpu/armv7/start.o (.text*) 
    *(.text*)
    } > ram
   
   . = ALIGN(4);

   .data : {
    __data_start__ = .;
    *(.data*)
   } > ram
   __data_end__ = .;
   __data_size__ = __data_end__ - __data_start__;

   . = ALIGN(4);

   .bss : {
    __bss_start__ = .;
    *(.bss*)
    } > ram
   __bss_end__ = .;
   __bss_size__ = __bss_end__ - __bss_start__;

   . = ALIGN(4); 

   /* setup byte pool start address */
   __bytepool_start = .;
   . += CONFIG_BYTEPOOL_SIZE;

   . = ALIGN(4);

   /* Move the stack pointer to top of the given ram  */
   __stack_start_ptr = ORIGIN(ram) + LENGTH(ram); 
}
