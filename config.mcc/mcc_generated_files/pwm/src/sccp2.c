/**
 * SCCP2 Generated Driver Source File
 * 
 * @file 	  sccp2.c
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP2 driver
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
#include "../sccp2.h"

// Section: File specific functions

static void (*SCCP2_PWMHandler)(void) = NULL;

// Section: Driver Interface

const struct PWM_INTERFACE PWM_LEDG = {
    .Initialize          = &SCCP2_PWM_Initialize,
    .Deinitialize        = &SCCP2_PWM_Deinitialize,
    .Enable              = &SCCP2_PWM_Enable,
    .Disable             = &SCCP2_PWM_Disable,
    .PeriodSet           = &SCCP2_PWM_PeriodSet,
    .DutyCycleSet        = &SCCP2_PWM_DutyCycleSet,
    .SoftwareTriggerSet  = &SCCP2_PWM_SoftwareTriggerSet,
    .DeadTimeSet         = NULL,
    .OutputModeSet       = NULL,
    .CallbackRegister = &SCCP2_PWM_CallbackRegister,
    .Tasks               = &SCCP2_PWM_Tasks
};

// Section: SCCP2 Module APIs

void SCCP2_PWM_Initialize (void)
{
    // MOD Dual Edge Compare, Buffered(PWM); CCSEL disabled; TMR32 16 Bit; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; CCPSLP disabled; CCPSIDL disabled; CCPON disabled; U
    CCP2CON1L = 0x5U;
    // SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each Time Base Period Match; RTRGEN disabled; OPSRC Timer Interrupt Event; U
    CCP2CON1H = 0x0U;
    // ASDG 0x0; SSDG disabled; ASDGM disabled; PWMRSEN disabled; U
    CCP2CON2L = 0x0U;
    // ICSEL ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN disabled; OENSYNC disabled; U
    CCP2CON2H = 0x0U;
    // PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; U
    CCP2CON3H = 0x0U;
    // ICOV disabled; ICDIS disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; U
    CCP2STATL = 0x0U;
    // TMRL 0x0000; U
    CCP2TMRL = 0x0U;
    // TMRH 0x0000; U
    CCP2TMRH = 0x0U;
    // PRL 50000; U
    CCP2PRL = 0xC350U;
    // PRH 0; U
    CCP2PRH = 0x0U;
    // CMPA 0; U
    CCP2RA = 0x0U;
    // CMPB 0; U
    CCP2RB = 0x0U;
    // BUFL 0x0000; U
    CCP2BUFL = 0x0U;
    // BUFH 0x0000; U
    CCP2BUFH = 0x0U;
    SCCP2_PWM_CallbackRegister(&SCCP2_PWM_Callback);
    
    CCP2CON1Lbits.CCPON = 1U; //Enable Module
}

void SCCP2_PWM_Deinitialize (void)
{
    CCP2CON1Lbits.CCPON = 0U;
    
    CCP2CON1L = 0x0U;
    CCP2CON1H = 0x0U;
    CCP2CON2L = 0x0U;
    CCP2CON2H = 0x100U;
    CCP2CON3H = 0x0U;
    CCP2STATL = 0x0U;
    CCP2TMRL = 0x0U;
    CCP2TMRH = 0x0U;
    CCP2PRL = 0xFFFFU;
    CCP2PRH = 0xFFFFU;
    CCP2RA = 0x0U;
    CCP2RB = 0x0U;
    CCP2BUFL = 0x0U;
    CCP2BUFH = 0x0U;
}

void SCCP2_PWM_Enable( void )
{
    CCP2CON1Lbits.CCPON = 1U;
}


void SCCP2_PWM_Disable( void )
{
    CCP2CON1Lbits.CCPON = 0U;
}

void SCCP2_PWM_PeriodSet(size_t periodCount)
{
    CCP2PRL = periodCount;
}

void SCCP2_PWM_DutyCycleSet(size_t dutyCycleCount)
{
    CCP2RB = dutyCycleCount;
}

void SCCP2_PWM_SoftwareTriggerSet( void )
{
    CCP2STATLbits.TRSET = 1U;
}

void SCCP2_PWM_CallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        SCCP2_PWMHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP2_PWM_Callback ( void )
{ 

} 


void SCCP2_PWM_Tasks( void )
{    
    if(IFS1bits.CCT2IF == 1)
    {
        // SCCP2 callback function 
        if(NULL != SCCP2_PWMHandler)
        {
            (*SCCP2_PWMHandler)();
        }
        IFS1bits.CCT2IF = 0U;
    }
}
/**
 End of File
*/
