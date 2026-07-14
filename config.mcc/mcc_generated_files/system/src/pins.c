 /**
 * PINS Generated Driver Source File 
 * 
 * @file      pins.c
 *            
 * @ingroup   pinsdriver
 *            
 * @brief     This is the generated driver source file for PINS driver.
 *
 * @skipline @version   Firmware Driver Version 1.0.2
 *
 * @skipline @version   PLIB Version 1.4.1
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

// Section: Includes
#include <xc.h>
#include <stddef.h>
#include "../pins.h"

// Section: File specific functions
static void (*FPGA_INT_InterruptHandler)(void) = NULL;

// Section: Driver Interface Function Definitions
void PINS_Initialize(void)
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000U;
    LATB = 0x0010U;
    LATC = 0x0800U;
    LATD = 0x0010U;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x001FU;
    TRISB = 0x07EFU;
    TRISC = 0x97FEU;
    TRISD = 0x9FCCU;


    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPUA = 0x0000U;
    CNPUB = 0x0060U;
    CNPUC = 0x0300U;
    CNPUD = 0x0000U;
    CNPDA = 0x0000U;
    CNPDB = 0xF000U;
    CNPDC = 0x0000U;
    CNPDD = 0x0023U;


    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000U;
    ODCB = 0x0000U;
    ODCC = 0x0000U;
    ODCD = 0x0000U;


    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSELA = 0x001DU;
    ANSELB = 0x008FU;
    ANSELC = 0x00CAU;
    ANSELD = 0x0C00U;

    /****************************************************************************
     * Set the PPS
     ***************************************************************************/
     __builtin_write_RPCON(0x0000); // unlock PPS

        RPINR18bits.U1RXR = 0x003AU; //RC10->UART1:U1RX;
        RPINR20bits.SDI1R = 0x0042U; //RD2->SPI1:SDI1;
        RPINR9bits.ICM7R = 0x004FU; //RD15->SCCP7:ICM7;
        RPINR19bits.U2RXR = 0x003CU; //RC12->UART2:U2RX;
        RPOR13bits.RP59R = 0x0001U;  //RC11->UART1:U1TX;
        RPOR15bits.RP62R = 0x0005U;  //RC14->SPI1:SDO1;
        RPOR6bits.RP44R = 0x0010U;  //RB12->SCCP2:OCM2;
        RPOR7bits.RP46R = 0x000FU;  //RB14->SCCP1:OCM1;
        RPOR6bits.RP45R = 0x0012U;  //RB13->SCCP4:OCM4;
        RPOR7bits.RP47R = 0x0011U;  //RB15->SCCP3:OCM3;
        RPOR16bits.RP64R = 0x0014U;  //RD0->SCCP6:OCM6;
        RPOR16bits.RP65R = 0x0013U;  //RD1->SCCP5:OCM5;
        RPOR2bits.RP36R = 0x0003U;  //RB4->UART2:U2TX;
        RPINR20bits.SCK1R = 0x003FU;  //RC15->SPI1:SCK1IN;
        RPOR15bits.RP63R = 0x0006U;  //RC15->SPI1:SCK1OUT;

     __builtin_write_RPCON(0x0800); // lock PPS

    /*******************************************************************************
    * Interrupt On Change: positive
    *******************************************************************************/
    CNEN0Dbits.CNEN0D9 = 1; //Pin : RD9U; 

    /****************************************************************************
     * Interrupt On Change: flag
     ***************************************************************************/
    CNFDbits.CNFD9 = 0;    //Pin : FPGA_INT

    /****************************************************************************
     * Interrupt On Change: config
     ***************************************************************************/
    CNCONDbits.CNSTYLE = 1; //Config for PORTD
    CNCONDbits.ON = 1; //Config for PORTD

    /* Initialize IOC Interrupt Handler*/
    FPGA_INT_SetInterruptHandler(&FPGA_INT_CallBack);

    /****************************************************************************
     * Interrupt On Change: Interrupt Enable
     ***************************************************************************/
    IFS4bits.CNDIF = 0; //Clear CNDI interrupt flag
    IEC4bits.CNDIE = 1; //Enable CNDI interrupt
}

void __attribute__ ((weak)) FPGA_INT_CallBack(void)
{

}

void FPGA_INT_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    IEC4bits.CNDIE = 0; //Disable CNDI interrupt
    FPGA_INT_InterruptHandler = InterruptHandler; 
    IEC4bits.CNDIE = 1; //Enable CNDI interrupt
}

/* Interrupt service function for the CNDI interrupt. */
/* cppcheck-suppress misra-c2012-8.4
*
* (Rule 8.4) REQUIRED: A compatible declaration shall be visible when an object or 
* function with external linkage is defined
*
* Reasoning: Interrupt declaration are provided by compiler and are available
* outside the driver folder
*/
void __attribute__ (( interrupt, no_auto_psv )) _CNDInterrupt (void)
{
    if(CNFDbits.CNFD9 == 1)
    {
        if(FPGA_INT_InterruptHandler != NULL) 
        { 
            FPGA_INT_InterruptHandler(); 
        }
        
        CNFDbits.CNFD9 = 0;  //Clear flag for Pin - FPGA_INT
    }
    
    // Clear the flag
    IFS4bits.CNDIF = 0;
}

