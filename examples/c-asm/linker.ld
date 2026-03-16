ENTRY(_start)
SECTIONS
{
  . = 0x80000000;    /* QEMU virt machine reset vector */
  .text : { 
    *(.text*)
    *(.text.startup)
  }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss : { 
    *(.bss*)
    *(COMMON)
  }
  
  /* Stack grows downward from this address */
  . = ALIGN(8);
  _stack_top = . + 0x20000;  /* 128KB stack (needed for chacha20 nested calls) */
}