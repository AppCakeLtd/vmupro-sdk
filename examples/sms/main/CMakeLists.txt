idf_component_register(
  SRCS
  "main.cpp"
  "smsplus/crc32.c"
  "smsplus/error.c"
  "smsplus/fileio.c"
  "smsplus/hvc.c"
  "smsplus/loadrom.c"
  "smsplus/memz80.c"
  "smsplus/pio.c"
  "smsplus/render.c"
  "smsplus/render_bp_lut.c"
  "smsplus/render_lut.c"
  "smsplus/sms.c"
  "smsplus/sms.c"
  "smsplus/sms.c"
  "smsplus/smsconfig.c"
  "smsplus/state.c"
  "smsplus/system.c"
  "smsplus/tms.c"
  "smsplus/tms_bp_expand.c"
  "smsplus/tms_lookup.c"
  "smsplus/tms_mc_lookup.c"
  "smsplus/tms_obj_lut.c"
  "smsplus/tms_txt_lookup.c"
  "smsplus/vdp.c"
  "smsplus/cpu/z80.c"
  "smsplus/cpu/z80_SZHVC_add_table.c"
  "smsplus/cpu/z80_SZHVC_sub_table.c"
  "smsplus/sound/emu2413.c"
  "smsplus/sound/fmintf.c"
  "smsplus/sound/sn76489.c"
  "smsplus/sound/sound.c"
  "smsplus/sound/emu2413.c"
  "smsplus/sound/ym2413.c"
  #"smsplus/unzip/ioapi.c"
  #"smsplus/unzip/unzip.c"

  # INCLUDE_DIRS "include"  
  PRIV_INCLUDE_DIRS "." "smsplus" "smsplus/cpu" "smsplus/sound"
)

target_compile_options(${COMPONENT_LIB} PRIVATE -nostartfiles -nostdlib -fno-builtin -Wno-unused-const-variable)
target_compile_definitions(${COMPONENT_LIB} PRIVATE LSB_FIRST=1)