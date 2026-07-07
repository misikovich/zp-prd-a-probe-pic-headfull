/**
 * SCCP6 Generated Driver Source File
 * 
 * @file 	  sccp6.c
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP6 driver
 *
 * @skipline @version   Firmware Driver Version 2.1.1
 *
 * @skipline @version   PLIB Version 1.6.6
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
#include "../sccp6.h"

// Section: File specific functions

static void (*SCCP6_PWMHandler)(void) = NULL;

// Section: Driver Interface

const struct PWM_INTERFACE PWM_SERVO = {
    .Initialize          = &SCCP6_PWM_Initialize,
    .Deinitialize        = &SCCP6_PWM_Deinitialize,
    .Enable              = &SCCP6_PWM_Enable,
    .Disable             = &SCCP6_PWM_Disable,
    .PeriodSet           = &SCCP6_PWM_PeriodSet,
    .DutyCycleSet        = &SCCP6_PWM_DutyCycleSet,
    .SoftwareTriggerSet  = &SCCP6_PWM_SoftwareTriggerSet,
    .DeadTimeSet         = NULL,
    .OutputModeSet       = NULL,
    .CallbackRegister = &SCCP6_PWM_CallbackRegister,
    .Tasks               = &SCCP6_PWM_Tasks
};

// Section: SCCP6 Module APIs

void SCCP6_PWM_Initialize (void)
{
    // MOD Dual Edge Compare, Buffered(PWM); CCSEL disabled; TMR32 16 Bit; TMRPS 1:64; CLKSEL FOSC/2; TMRSYNC disabled; CCPSLP disabled; CCPSIDL disabled; CCPON disabled; U
    CCP6CON1L = 0xC5U;
    // SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each Time Base Period Match; RTRGEN disabled; OPSRC Timer Interrupt Event; U
    CCP6CON1H = 0x0U;
    // ASDG 0x0; SSDG disabled; ASDGM disabled; PWMRSEN disabled; U
    CCP6CON2L = 0x0U;
    // ICSEL ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN enabled; OENSYNC disabled; U
    CCP6CON2H = 0x100U;
    // PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; U
    CCP6CON3H = 0x0U;
    // ICOV disabled; ICDIS disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; U
    CCP6STATL = 0x0U;
    // TMRL 0x0000; U
    CCP6TMRL = 0x0U;
    // TMRH 0x0000; U
    CCP6TMRH = 0x0U;
    // PRL 31250; U
    CCP6PRL = 0x7A12U;
    // PRH 0; U
    CCP6PRH = 0x0U;
    // CMPA 0; U
    CCP6RA = 0x0U;
    // CMPB 0; U
    CCP6RB = 0x0U;
    // BUFL 0x0000; U
    CCP6BUFL = 0x0U;
    // BUFH 0x0000; U
    CCP6BUFH = 0x0U;
    SCCP6_PWM_CallbackRegister(&SCCP6_PWM_Callback);
    
    CCP6CON1Lbits.CCPON = 1U; //Enable Module
}

void SCCP6_PWM_Deinitialize (void)
{
    CCP6CON1Lbits.CCPON = 0U;
    
    CCP6CON1L = 0x0U;
    CCP6CON1H = 0x0U;
    CCP6CON2L = 0x0U;
    CCP6CON2H = 0x100U;
    CCP6CON3H = 0x0U;
    CCP6STATL = 0x0U;
    CCP6TMRL = 0x0U;
    CCP6TMRH = 0x0U;
    CCP6PRL = 0xFFFFU;
    CCP6PRH = 0xFFFFU;
    CCP6RA = 0x0U;
    CCP6RB = 0x0U;
    CCP6BUFL = 0x0U;
    CCP6BUFH = 0x0U;
}

void SCCP6_PWM_Enable( void )
{
    CCP6CON1Lbits.CCPON = 1U;
}


void SCCP6_PWM_Disable( void )
{
    CCP6CON1Lbits.CCPON = 0U;
}

void SCCP6_PWM_PeriodSet(size_t periodCount)
{
    CCP6PRL = periodCount;
}

void SCCP6_PWM_DutyCycleSet(size_t dutyCycleCount)
{
    CCP6RB = dutyCycleCount;
}

void SCCP6_PWM_SoftwareTriggerSet( void )
{
    CCP6STATLbits.TRSET = 1U;
}

void SCCP6_PWM_CallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        SCCP6_PWMHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP6_PWM_Callback ( void )
{ 

} 


void SCCP6_PWM_Tasks( void )
{    
    if(IFS2bits.CCT6IF == 1)
    {
        // SCCP6 callback function 
        if(NULL != SCCP6_PWMHandler)
        {
            (*SCCP6_PWMHandler)();
        }
        IFS2bits.CCT6IF = 0U;
    }
}
/**
 End of File
*/
