/*
 * Context switch (vPortYield) for the dsPIC33C core, XC16.
 *
 * Derived from the FreeRTOS Kernel V11.2.0 portasm_dsPIC.S
 * (Copyright (C) 2021 Amazon.com, Inc. or its affiliates,
 * SPDX-License-Identifier: MIT — see third_party/FreeRTOS-Kernel/LICENSE.md).
 *
 * dsPIC33C differences vs the stock dsPIC30F/33F version: EDS paging
 * registers DSRPAG/DSWPAG are saved instead of PSVPAG.  The frame layout
 * must match portRESTORE_CONTEXT() and pxPortInitialiseStack() in port.c.
 */

#if defined( __dsPIC33C__ )

        .global _vPortYield
        .extern _vTaskSwitchContext
        .extern _uxCriticalNesting

_vPortYield:

        PUSH    SR                      /* Save the SR used by the task.... */
        PUSH    W0                      /* ....then disable interrupts. */
        MOV     #32, W0                 /* IPL = configKERNEL_INTERRUPT_PRIORITY (1). */
        MOV     W0, SR
        PUSH    W1                      /* Save registers to the stack. */
        PUSH.D  W2
        PUSH.D  W4
        PUSH.D  W6
        PUSH.D  W8
        PUSH.D  W10
        PUSH.D  W12
        PUSH    W14
        PUSH    RCOUNT
        PUSH    TBLPAG
        PUSH    ACCAL
        PUSH    ACCAH
        PUSH    ACCAU
        PUSH    ACCBL
        PUSH    ACCBH
        PUSH    ACCBU
        PUSH    DCOUNT
        PUSH    DOSTARTL
        PUSH    DOSTARTH
        PUSH    DOENDL
        PUSH    DOENDH

        PUSH    CORCON
        PUSH    DSRPAG
        PUSH    DSWPAG
        MOV     _uxCriticalNesting, W0      /* Save the critical nesting counter for the task. */
        PUSH    W0
        MOV     _pxCurrentTCB, W0           /* Save the new top of stack into the TCB. */
        MOV     W15, [W0]

        call    _vTaskSwitchContext

        MOV     _pxCurrentTCB, W0           /* Restore the stack pointer for the task. */
        MOV     [W0], W15
        POP     W0                          /* Restore the critical nesting counter for the task. */
        MOV     W0, _uxCriticalNesting
        POP     DSWPAG
        POP     DSRPAG
        POP     CORCON
        POP     DOENDH
        POP     DOENDL
        POP     DOSTARTH
        POP     DOSTARTL
        POP     DCOUNT
        POP     ACCBU
        POP     ACCBH
        POP     ACCBL
        POP     ACCAU
        POP     ACCAH
        POP     ACCAL
        POP     TBLPAG
        POP     RCOUNT                      /* Restore the registers from the stack. */
        POP     W14
        POP.D   W12
        POP.D   W10
        POP.D   W8
        POP.D   W6
        POP.D   W4
        POP.D   W2
        POP.D   W0
        POP     SR

        return

        .end

#endif /* defined( __dsPIC33C__ ) */
