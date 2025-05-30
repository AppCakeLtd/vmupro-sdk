ENTRY(vmupro_main)

MEMORY
{
  iram (rx) : ORIGIN = 0x00000000, LENGTH = 0x2800  /* 10KB IRAM (loader loads at address 0) */
}

SECTIONS
{
  .text : ALIGN(4)
  {
    _text_start = .;
    *(.literal*)
    *(.text*)
    _text_end = .;
  } > iram

  .rodata : ALIGN(4)
  {
    _rodata_start = .;
    *(.rodata*)
    *(.rodata.*)
    _rodata_end = .;
  } > iram

  .data : ALIGN(4)
  {
    _data_start = .;
    *(.data*)
    _data_end = .;
  } > iram

  .bss (NOLOAD) : ALIGN(4)
  {
    _bss_start = .;
    *(.bss*)
    *(COMMON)
    _bss_end = .;
  } > iram
}
