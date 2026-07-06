/**
 * SCCP5 Generated Driver Source File
 * 
 * @file 	  sccp5.c
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP5 driver
 *
 * @skipline @version   Firmware Driver Version 2.1.1
 *
 * @skipline @version   PLIB Version 1.6.7
 *
 * @skipline  Device : dsPIC33CK256MP506
*/

/*
© [2026] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

// Section: Included Files

#include <xc.h>
#include <stddef.h>
#include "../sccp5.h"

// Section: File specific functions

static void (*SCCP5_PWMHandler)(void) = NULL;

// Section: Driver Interface

const struct PWM_INTERFACE PWM_SOCKETW = {
    .Initialize          = &SCCP5_PWM_Initialize,
    .Deinitialize        = &SCCP5_PWM_Deinitialize,
    .Enable              = &SCCP5_PWM_Enable,
    .Disable             = &SCCP5_PWM_Disable,
    .PeriodSet           = &SCCP5_PWM_PeriodSet,
    .DutyCycleSet        = &SCCP5_PWM_DutyCycleSet,
    .SoftwareTriggerSet  = &SCCP5_PWM_SoftwareTriggerSet,
    .DeadTimeSet         = NULL,
    .OutputModeSet       = NULL,
    .CallbackRegister = &SCCP5_PWM_CallbackRegister,
    .Tasks               = &SCCP5_PWM_Tasks
};

// Section: SCCP5 Module APIs

void SCCP5_PWM_Initialize (void)
{
    // MOD Dual Edge Compare, Buffered(PWM); CCSEL disabled; TMR32 16 Bit; TMRPS 1:16; CLKSEL FOSC/2; TMRSYNC disabled; CCPSLP disabled; CCPSIDL disabled; CCPON disabled; U
    CCP5CON1L = 0x85U;
    // SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each Time Base Period Match; RTRGEN disabled; OPSRC Timer Interrupt Event; U
    CCP5CON1H = 0x0U;
    // ASDG 0x0; SSDG disabled; ASDGM disabled; PWMRSEN disabled; U
    CCP5CON2L = 0x0U;
    // ICSEL ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN disabled; OENSYNC disabled; U
    CCP5CON2H = 0x0U;
    // PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; U
    CCP5CON3H = 0x0U;
    // ICOV disabled; ICDIS disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; U
    CCP5STATL = 0x0U;
    // TMRL 0x0000; U
    CCP5TMRL = 0x0U;
    // TMRH 0x0000; U
    CCP5TMRH = 0x0U;
    // PRL 62500; U
    CCP5PRL = 0xF424U;
    // PRH 0; U
    CCP5PRH = 0x0U;
    // CMPA 0; U
    CCP5RA = 0x0U;
    // CMPB 31250; U
    CCP5RB = 0x7A12U;
    // BUFL 0x0000; U
    CCP5BUFL = 0x0U;
    // BUFH 0x0000; U
    CCP5BUFH = 0x0U;
    SCCP5_PWM_CallbackRegister(&SCCP5_PWM_Callback);
    
    CCP5CON1Lbits.CCPON = 1U; //Enable Module
}

void SCCP5_PWM_Deinitialize (void)
{
    CCP5CON1Lbits.CCPON = 0U;
    
    CCP5CON1L = 0x0U;
    CCP5CON1H = 0x0U;
    CCP5CON2L = 0x0U;
    CCP5CON2H = 0x100U;
    CCP5CON3H = 0x0U;
    CCP5STATL = 0x0U;
    CCP5TMRL = 0x0U;
    CCP5TMRH = 0x0U;
    CCP5PRL = 0xFFFFU;
    CCP5PRH = 0xFFFFU;
    CCP5RA = 0x0U;
    CCP5RB = 0x0U;
    CCP5BUFL = 0x0U;
    CCP5BUFH = 0x0U;
}

void SCCP5_PWM_Enable( void )
{
    CCP5CON1Lbits.CCPON = 1U;
}


void SCCP5_PWM_Disable( void )
{
    CCP5CON1Lbits.CCPON = 0U;
}

void SCCP5_PWM_PeriodSet(size_t periodCount)
{
    CCP5PRL = periodCount;
}

void SCCP5_PWM_DutyCycleSet(size_t dutyCycleCount)
{
    CCP5RB = dutyCycleCount;
}

void SCCP5_PWM_SoftwareTriggerSet( void )
{
    CCP5STATLbits.TRSET = 1U;
}

void SCCP5_PWM_CallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        SCCP5_PWMHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP5_PWM_Callback ( void )
{ 

} 


void SCCP5_PWM_Tasks( void )
{    
    if(IFS2bits.CCT5IF == 1)
    {
        // SCCP5 callback function 
        if(NULL != SCCP5_PWMHandler)
        {
            (*SCCP5_PWMHandler)();
        }
        IFS2bits.CCT5IF = 0U;
    }
}
/**
 End of File
*/
