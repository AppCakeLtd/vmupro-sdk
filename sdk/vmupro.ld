ENTRY(vmupro_main)

MEMORY
{
  psram (rxw) : ORIGIN = 0x00000000, LENGTH = 0x400000  /* 4MB */
}

SECTIONS
{
  .text : ALIGN(4)
  {
    _text_start = .;
    *(.literal*)
    *(.text*)
    _text_end = .;
  } > psram

  .rodata : ALIGN(4)
  {
    _rodata_start = .;
    *(.rodata*)
    *(.rodata.*)
    _rodata_end = .;
  } > psram

  .data : ALIGN(4)
  {
    _data_start = .;
    *(.data*)
    _data_end = .;
  } > psram

  .bss : ALIGN(4)
  {
    _bss_start = .;
    *(.bss*)
    *(COMMON)
    _bss_end = .;
  } > psram
}
