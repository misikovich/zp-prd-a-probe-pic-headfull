# FreeRTOS Kernel (vendored)

Subset of the FreeRTOS Kernel **V11.2.0** (MIT license, see `LICENSE.md`),
vendored unmodified from https://github.com/FreeRTOS/FreeRTOS-Kernel.

Included: core kernel sources, `include/`, `portable/MemMang/heap_4.c`, and
`portable/MPLAB/PIC24_dsPIC/portmacro.h`.

The stock `PIC24_dsPIC` port does not support the dsPIC33C core, so the
device port (`port.c` + context-switch asm, derived from the stock port)
lives in `rtos/port/`. Build wiring is in
`cmake/zp-prd-a-probe-pic-headfull/default/user.cmake` — when upgrading the
kernel, refresh these files from the new tag and re-check `rtos/port/`
against the upstream `PIC24_dsPIC` port for changes.
