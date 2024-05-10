cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

project(nixie-tube-clock)

if (TEST)
  add_executable(clock-test
    test/framework.c
    test/mock.c

    test/avr_mock.c
    test/led_mock.c
    test/wifi_mock.c

    test/clock_tc.c

    clock.c
  )

  target_compile_options(clock-test PUBLIC
    -g
    -std=c99
    #-pedantic
    -Wall
    -Werror)

  target_include_directories(clock-test PUBLIC . test)

  add_custom_command(
    TARGET clock-test
    POST_BUILD
    COMMAND clock-test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

  add_executable(led-test
    test/framework.c
    test/mock.c

    test/avr_mock.c
    test/clock_mock.c

    test/led_tc.c

    led.c
  )

  target_compile_options(led-test PUBLIC
    -g
    -std=c99
    #-pedantic
    -Wall
    -Werror)

  target_include_directories(led-test PUBLIC . test)

  add_custom_command(
    TARGET led-test
    POST_BUILD
    COMMAND led-test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

  add_executable(wifi-test
    test/framework.c
    test/mock.c

    test/avr_mock.c
    test/clock_mock.c

    test/wifi_tc.c

    wifi.c
  )

  target_compile_options(wifi-test PUBLIC
    -g
    -std=c99
    #-pedantic
    -Wall
    -Werror)

  target_compile_definitions(wifi-test PUBLIC
    -DWIFI_SSID="wifi-ssid"
    -DWIFI_PASSWD="wifi-pass"
    -DESP_TCP_PORT="esp-tcp-port"
    -DTIME_SERVER_IP="time-server-ip"
    -DTIME_SERVER_PORT="time-server-port"
  )

  target_include_directories(wifi-test PUBLIC . test)

  add_custom_command(
    TARGET wifi-test
    POST_BUILD
    COMMAND wifi-test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

else()
  set(F_CPU 7372800UL)

  # CPU, you can find the list here:
  # https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
  set(MCU_GCC atmega8a)
  set(MCU_DUDE m8)
  # The programmer to use, read avrdude manual for list
  set(PROG_TYPE avrispmkII)

  set(WIFI_SSID xxxxx)
  set(WIFI_PASSWD xxxxx)

  set(ESP_TCP_PORT xxxx)

  set(TIME_SERVER_IP xxxx)
  set(TIME_SERVER_PORT xxxx)

  # AVR Fuses, must be in concordance with your hardware and F_CPU
  # https://www.engbedded.com/fusecalc/
  set(L_FUSE 0xfd)
  set(H_FUSE 0xd9)

  set(CMAKE_SYSTEM_NAME Generic)
  set(CMAKE_CXX_COMPILER avr-g++)
  set(CMAKE_C_COMPILER avr-gcc)
  set(CMAKE_ASM_COMPILER avr-gcc)

  add_executable(nixie-tube-clock
    avr.c
    main.c
    clock.c
    led.c
    wifi.c
  )

  target_compile_definitions(nixie-tube-clock PUBLIC
    -DF_CPU=${F_CPU}
    -DWIFI_SSID="${WIFI_SSID}"
    -DWIFI_PASSWD="${WIFI_PASSWD}"
    -DESP_TCP_PORT="${ESP_TCP_PORT}"
    -DTIME_SERVER_IP="${TIME_SERVER_IP}"
    -DTIME_SERVER_PORT="${TIME_SERVER_PORT}"
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
    -Wl,--relax,--gc-sections
    -fpack-struct
    -fshort-enums
    -ffunction-sections
    -fdata-sections
  )

  target_link_options(nixie-tube-clock PUBLIC -mmcu=${MCU_GCC})

  target_link_libraries(nixie-tube-clock PUBLIC -lm)

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
  # -U lfuse:w:0xfd:m -U hfuse:w:0xd9:m
  add_custom_target(fuses avrdude -c ${PROG_TYPE} -p ${MCU_DUDE} -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m)

  # Clean extra files
  set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")
endif()
