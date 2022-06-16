cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

project(nixie-tube-clock)

# 8MHz, this should match the crystal on your board,
# I use 8MHz and 3.3V for the lowest power consumption
set(F_CPU 7372800UL)
# CPU, you can find the list here:
# https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
set(MCU_GCC atmega88pa)
set(MCU_DUDE m88p)
# Default Baudrate for UART, read avr include/util/setbaud.h for usage
set(BAUD 115200)
# The programmer to use, read avrdude manual for list
set(PROG_TYPE avrispmkII)

set(WIFI_SSID xxxx)
set(WIFI_PASSWD xxxx)

# AVR Fuses, must be in concordance with your hardware and F_CPU
# https://www.engbedded.com/fusecalc/
set(E_FUSE 0xfd)
set(H_FUSE 0xda)
set(L_FUSE 0xfd)
set(LOCK_BIT 0xff)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)

add_executable(nixie-tube-clock
  main.c
  clock.c
  led.c
  wifi.c
)

target_compile_definitions(nixie-tube-clock PUBLIC
    -DF_CPU=${F_CPU}
    -DBAUD=${BAUD}
    -DWIFI_SSID=${WIFI_SSID}
    -DWIFI_PASSWD=${WIFI_PASSWD}
)

target_include_directories(nixie-tube-clock PUBLIC
  /usr/lib/avr/include
)

target_compile_options(nixie-tube-clock PUBLIC
    -mmcu=${MCU_GCC}
    -std=gnu99
    -Os
    -Wall
    -Wundef
    -pedantic
    -Werror
    -Wfatal-errors
    -Wl,--relax,--gc-sections
    -funsigned-char
    -funsigned-bitfields
    -fpack-struct
    -fshort-enums
    -ffunction-sections
    -fdata-sections
)

target_link_options(nixie-tube-clock PUBLIC -mmcu=${MCU_GCC})

# Rename the output to .elf as we will create multiple files
set_target_properties(nixie-tube-clock PROPERTIES OUTPUT_NAME nixie-tube-clock.elf)

add_custom_command(TARGET nixie-tube-clock POST_BUILD
  DEPENDS $<TARGET_FILE:nixie-tube-clock>
  COMMAND avr-size -C $<TARGET_FILE:nixie-tube-clock> --mcu=${MCU_GCC}
)

# Transform binary into hex file, we ignore the eeprom segments in the step
add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex nixie-tube-clock.elf nixie-tube-clock.hex DEPENDS nixie-tube-clock)
# Transform binary into hex file, this is the eeprom part (empty if you don't
# use eeprom static variables)
add_custom_target(eeprom avr-objcopy -j .eeprom  --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.eep DEPENDS strip)

# Upload the firmware with avrdude
add_custom_target(upload avrdude  -c ${PROG_TYPE} -p ${MCU_DUDE} -U flash:w:nixie-tube-clock.hex DEPENDS hex)

# Upload the eeprom with avrdude
add_custom_target(upload_eeprom avrdude -c ${PROG_TYPE} -p ${MCU}  -U eeprom:w:${PRODUCT_NAME}.eep DEPENDS eeprom)

# Burn fuses
# -U lfuse:w:0xfd:m -U hfuse:w:0xdf:m -U efuse:w:0xf9:m
add_custom_target(fuses avrdude -c ${PROG_TYPE} -p ${MCU}  -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m -U efuse:w:${E_FUSE}:m -U lock:w:${LOCK_BIT}:m )

# Clean extra files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")
