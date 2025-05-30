ENTRY(vmupro_main)

MEMORY
{
  iram (rx) : ORIGIN = 0x0, LENGTH = 0x2800
}

SECTIONS
{
  .text : ALIGN(4)
  {
    *(.literal*)
    *(.text*)
  }

  .rodata : ALIGN(4)
  {
    *(.rodata*)
    *(.rodata.*)
  }

  .data : ALIGN(4)
  {
    *(.data*)
  }

  .bss (NOLOAD) : ALIGN(4)
  {
    *(.bss*)
    *(COMMON)
  }
}
