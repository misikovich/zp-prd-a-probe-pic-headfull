# FreeRTOS kernel and dsPIC33C port.
#
# Sources are added here (not in the MPLAB fileSet) because MPLAB regenerates
# .generated/file.cmake and would drop hand-added entries.  third_party/ and
# rtos/ are excluded from the fileSet glob in
# .vscode/zp-prd-a-probe-pic-headfull.mplab.json to avoid double compilation.

set(zp_freertos_dir "${CMAKE_CURRENT_LIST_DIR}/../../../third_party/FreeRTOS-Kernel")
set(zp_rtos_dir "${CMAKE_CURRENT_LIST_DIR}/../../../rtos")

set(zp_drivers_dir "${CMAKE_CURRENT_LIST_DIR}/../../../drivers")
set(zp_config_mcc_dir "${CMAKE_CURRENT_LIST_DIR}/../../../config.mcc")
set(zp_protocol_dir "${CMAKE_CURRENT_LIST_DIR}/../../../protocol")

set(zp_freertos_includes
    "${zp_freertos_dir}/include"
    "${zp_freertos_dir}/portable/MPLAB/PIC24_dsPIC"
    "${zp_rtos_dir}"
    "${zp_drivers_dir}"
    "${zp_config_mcc_dir}"
    "${zp_protocol_dir}")

# C sources join the generated compile object library so they get the same
# flags as the rest of the firmware.
target_sources(zp_prd_a_probe_pic_headfull_default_toolchain_XC16_2_10_compile PRIVATE
    "${zp_freertos_dir}/tasks.c"
    "${zp_freertos_dir}/list.c"
    "${zp_freertos_dir}/queue.c"
    "${zp_freertos_dir}/timers.c"
    "${zp_freertos_dir}/event_groups.c"
    "${zp_freertos_dir}/stream_buffer.c"
    "${zp_freertos_dir}/portable/MemMang/heap_4.c"
    "${zp_rtos_dir}/port/port.c"
    "${zp_rtos_dir}/rtos_hooks.c"
    "${zp_drivers_dir}/spi_bus.c"
    "${zp_drivers_dir}/emeter.c"
    "${zp_drivers_dir}/emeter_service.c"
    "${zp_drivers_dir}/fpga.c"
    "${zp_drivers_dir}/fpga_service.c"
    "${zp_drivers_dir}/rgbw.c"
    "${zp_drivers_dir}/esp_uart.c"
    "${zp_drivers_dir}/wproto.c")
target_include_directories(zp_prd_a_probe_pic_headfull_default_toolchain_XC16_2_10_compile
    PRIVATE ${zp_freertos_includes})

# The context-switch asm needs the C preprocessor (assemblePreproc group).
# That group is empty in the generated build, so make our own object library
# with the same rule and attach it to the image target (whose name carries a
# generated hash, hence the lookup).
add_library(zp_freertos_portasm OBJECT "${zp_rtos_dir}/port/portasm_dsPIC33C.S")
zp_prd_a_probe_pic_headfull_default_toolchain_XC16_2_10_assemblePreproc_rule(zp_freertos_portasm)

get_directory_property(zp_buildsystem_targets BUILDSYSTEM_TARGETS)
foreach(zp_target IN LISTS zp_buildsystem_targets)
    if(zp_target MATCHES "_image_")
        target_sources(${zp_target} PRIVATE "$<TARGET_OBJECTS:zp_freertos_portasm>")
    endif()
endforeach()
