/**
 * SCCP7-InputCapture Generated Driver Source File
 * 
 * @file 	  sccp7.c
 * 
 * @ingroup   mccpdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP7-InputCapture driver
 *
 * @skipline @version   Firmware Driver Version 1.3.0
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
#include "../sccp7.h"

// Section: File specific functions

static void (*SCCP7_InputCaptureHandler)(void) = NULL;

// Section: Driver Interface

const struct INPUT_CAPTURE_INTERFACE RCD_PWM_READ = {
    .Initialize          = &SCCP7_InputCapture_Initialize,
    .Deinitialize        = &SCCP7_InputCapture_Deinitialize,
    .Start               = &SCCP7_InputCapture_Start,
    .Stop                = &SCCP7_InputCapture_Stop,
    .InputCapture_CallbackRegister = &SCCP7_InputCapture_CallbackRegister,
    .Tasks               = &SCCP7_InputCapture_Tasks,
    .DataRead            = &SCCP7_InputCapture_DataRead,
    .HasBufferOverflowed = &SCCP7_InputCapture_HasBufferOverflowed,
    .IsBufferEmpty       = &SCCP7_InputCapture_IsBufferEmpty,
    .OverflowFlagClear   = &SCCP7_InputCapture_OverflowFlagClear,
};

// Section: Driver Interface Function Definitions

void SCCP7_InputCapture_Initialize(void)
{
    // MOD None; CCSEL enabled; TMR32 16 Bit; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; CCPSLP disabled; CCPSIDL disabled; CCPON disabled; 
    CCP7CON1L = 0x10U; //The module is disabled, till other settings are configured.
    //SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each Time Base Period Match; RTRGEN disabled; OPSRC Timer Interrupt Event; 
    CCP7CON1H = 0x0U;
    //ASDG 0x0; SSDG disabled; ASDGM disabled; PWMRSEN disabled; 
    CCP7CON2L = 0x0U;
    //ICSEL None; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN disabled; OENSYNC disabled; 
    CCP7CON2H = 0x0U;
    //PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; 
    CCP7CON3H = 0x0U;
    //ICOV disabled; ICDIS disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; 
    CCP7STATL = 0x0U;
    //TMRL 0x0000; 
    CCP7TMRL = 0x0U;
    //TMRH 0x0000; 
    CCP7TMRH = 0x0U;
    //PRL 0; 
    CCP7PRL = 0x0U;
    //PRH 0; 
    CCP7PRH = 0x0U;
    //CMPA 0; 
    CCP7RA = 0x0U;
    //CMPB 0; 
    CCP7RB = 0x0U;
    //BUFL 0x0000; 
    CCP7BUFL = 0x0U;
    //BUFH 0x0000; 
    CCP7BUFH = 0x0U;
    
    SCCP7_InputCapture_CallbackRegister(&SCCP7_InputCapture_Callback);


    CCP7CON1Lbits.CCPON = 1; //Enable Module

}

void SCCP7_InputCapture_Deinitialize(void)
{
    CCP7CON1Lbits.CCPON = 0;
    
    
    CCP7CON1L = 0x0U;
    CCP7CON1H = 0x0U;
    CCP7CON2L = 0x0U;
    CCP7CON2H = 0x100U;
    CCP7CON3H = 0x0U;
    CCP7STATL = 0x0U;
    CCP7TMRL = 0x0U;
    CCP7TMRH = 0x0U;
    CCP7PRL = 0xFFFFU;
    CCP7PRH = 0xFFFFU;
    CCP7RA = 0x0U;
    CCP7RB = 0x0U;
    CCP7BUFL = 0x0U;
    CCP7BUFH = 0x0U;
}

void SCCP7_InputCapture_Start(void)
{
    
    CCP7CON1Lbits.CCPON = 1;
}

void SCCP7_InputCapture_Stop(void)
{
    CCP7CON1Lbits.CCPON = 0;
    
}

void SCCP7_InputCapture_CallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        SCCP7_InputCaptureHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP7_InputCapture_Callback (void)
{ 

} 

void SCCP7_InputCapture_Tasks(void)
{
    if(IFS9bits.CCP7IF == 1)
    {
        if(NULL != SCCP7_InputCaptureHandler)
        {
            (*SCCP7_InputCaptureHandler)();
        }
        IFS9bits.CCP7IF = 0;
    }
}

uint32_t SCCP7_InputCapture_DataRead(void)
{
    uint32_t captureVal = 0xFFFFFFFFU;

    captureVal = CCP7BUFL;
    captureVal |= ((uint32_t)CCP7BUFH <<16);

    return(captureVal);
}

bool SCCP7_InputCapture_HasBufferOverflowed(void)
{
    return(CCP7STATLbits.ICOV);
}

bool SCCP7_InputCapture_IsBufferEmpty(void)
{
    return(!CCP7STATLbits.ICBNE);
}

void SCCP7_InputCapture_OverflowFlagClear(void)
{
    CCP7STATLbits.ICOV = 0;
}

/**
 End of File
*/
