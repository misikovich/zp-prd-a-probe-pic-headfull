/**
 * PINS Generated Driver Header File 
 * 
 * @file      pins.h
 *            
 * @defgroup  pinsdriver Pins Driver
 *            
 * @brief     The Pin Driver directs the operation and function of 
 *            the selected device pins using dsPIC MCUs.
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

#ifndef PINS_H
#define PINS_H
// Section: Includes
#include <xc.h>

// Section: Device Pin Macros

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA1 GPIO Pin which has a custom name of EM_ALM to High
 * @pre      The RA1 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define EM_ALM_SetHigh()          (_LATA1 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA1 GPIO Pin which has a custom name of EM_ALM to Low
 * @pre      The RA1 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define EM_ALM_SetLow()           (_LATA1 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RA1 GPIO Pin which has a custom name of EM_ALM
 * @pre      The RA1 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define EM_ALM_Toggle()           (_LATA1 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RA1 GPIO Pin which has a custom name of EM_ALM
 * @param    none
 * @return   none  
 */
#define EM_ALM_GetValue()         _RA1

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA1 GPIO Pin which has a custom name of EM_ALM as Input
 * @param    none
 * @return   none  
 */
#define EM_ALM_SetDigitalInput()  (_TRISA1 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA1 GPIO Pin which has a custom name of EM_ALM as Output
 * @param    none
 * @return   none  
 */
#define EM_ALM_SetDigitalOutput() (_TRISA1 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB5 GPIO Pin which has a custom name of BCD4 to High
 * @pre      The RB5 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define BCD4_SetHigh()          (_LATB5 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB5 GPIO Pin which has a custom name of BCD4 to Low
 * @pre      The RB5 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define BCD4_SetLow()           (_LATB5 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB5 GPIO Pin which has a custom name of BCD4
 * @pre      The RB5 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define BCD4_Toggle()           (_LATB5 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB5 GPIO Pin which has a custom name of BCD4
 * @param    none
 * @return   none  
 */
#define BCD4_GetValue()         _RB5

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB5 GPIO Pin which has a custom name of BCD4 as Input
 * @param    none
 * @return   none  
 */
#define BCD4_SetDigitalInput()  (_TRISB5 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB5 GPIO Pin which has a custom name of BCD4 as Output
 * @param    none
 * @return   none  
 */
#define BCD4_SetDigitalOutput() (_TRISB5 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB6 GPIO Pin which has a custom name of BCD8 to High
 * @pre      The RB6 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define BCD8_SetHigh()          (_LATB6 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB6 GPIO Pin which has a custom name of BCD8 to Low
 * @pre      The RB6 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define BCD8_SetLow()           (_LATB6 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB6 GPIO Pin which has a custom name of BCD8
 * @pre      The RB6 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define BCD8_Toggle()           (_LATB6 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB6 GPIO Pin which has a custom name of BCD8
 * @param    none
 * @return   none  
 */
#define BCD8_GetValue()         _RB6

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB6 GPIO Pin which has a custom name of BCD8 as Input
 * @param    none
 * @return   none  
 */
#define BCD8_SetDigitalInput()  (_TRISB6 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB6 GPIO Pin which has a custom name of BCD8 as Output
 * @param    none
 * @return   none  
 */
#define BCD8_SetDigitalOutput() (_TRISB6 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB11 GPIO Pin which has a custom name of FPGA_CS to High
 * @pre      The RB11 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define FPGA_CS_SetHigh()          (_LATB11 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB11 GPIO Pin which has a custom name of FPGA_CS to Low
 * @pre      The RB11 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define FPGA_CS_SetLow()           (_LATB11 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB11 GPIO Pin which has a custom name of FPGA_CS
 * @pre      The RB11 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define FPGA_CS_Toggle()           (_LATB11 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB11 GPIO Pin which has a custom name of FPGA_CS
 * @param    none
 * @return   none  
 */
#define FPGA_CS_GetValue()         _RB11

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB11 GPIO Pin which has a custom name of FPGA_CS as Input
 * @param    none
 * @return   none  
 */
#define FPGA_CS_SetDigitalInput()  (_TRISB11 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB11 GPIO Pin which has a custom name of FPGA_CS as Output
 * @param    none
 * @return   none  
 */
#define FPGA_CS_SetDigitalOutput() (_TRISB11 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC0 GPIO Pin which has a custom name of EM_RST to High
 * @pre      The RC0 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define EM_RST_SetHigh()          (_LATC0 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC0 GPIO Pin which has a custom name of EM_RST to Low
 * @pre      The RC0 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define EM_RST_SetLow()           (_LATC0 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RC0 GPIO Pin which has a custom name of EM_RST
 * @pre      The RC0 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define EM_RST_Toggle()           (_LATC0 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RC0 GPIO Pin which has a custom name of EM_RST
 * @param    none
 * @return   none  
 */
#define EM_RST_GetValue()         _RC0

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC0 GPIO Pin which has a custom name of EM_RST as Input
 * @param    none
 * @return   none  
 */
#define EM_RST_SetDigitalInput()  (_TRISC0 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC0 GPIO Pin which has a custom name of EM_RST as Output
 * @param    none
 * @return   none  
 */
#define EM_RST_SetDigitalOutput() (_TRISC0 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC1 GPIO Pin which has a custom name of TEMP2 to High
 * @pre      The RC1 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define TEMP2_SetHigh()          (_LATC1 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC1 GPIO Pin which has a custom name of TEMP2 to Low
 * @pre      The RC1 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define TEMP2_SetLow()           (_LATC1 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RC1 GPIO Pin which has a custom name of TEMP2
 * @pre      The RC1 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define TEMP2_Toggle()           (_LATC1 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RC1 GPIO Pin which has a custom name of TEMP2
 * @param    none
 * @return   none  
 */
#define TEMP2_GetValue()         _RC1

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC1 GPIO Pin which has a custom name of TEMP2 as Input
 * @param    none
 * @return   none  
 */
#define TEMP2_SetDigitalInput()  (_TRISC1 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC1 GPIO Pin which has a custom name of TEMP2 as Output
 * @param    none
 * @return   none  
 */
#define TEMP2_SetDigitalOutput() (_TRISC1 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC2 GPIO Pin which has a custom name of EM_INT to High
 * @pre      The RC2 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define EM_INT_SetHigh()          (_LATC2 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC2 GPIO Pin which has a custom name of EM_INT to Low
 * @pre      The RC2 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define EM_INT_SetLow()           (_LATC2 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RC2 GPIO Pin which has a custom name of EM_INT
 * @pre      The RC2 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define EM_INT_Toggle()           (_LATC2 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RC2 GPIO Pin which has a custom name of EM_INT
 * @param    none
 * @return   none  
 */
#define EM_INT_GetValue()         _RC2

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC2 GPIO Pin which has a custom name of EM_INT as Input
 * @param    none
 * @return   none  
 */
#define EM_INT_SetDigitalInput()  (_TRISC2 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC2 GPIO Pin which has a custom name of EM_INT as Output
 * @param    none
 * @return   none  
 */
#define EM_INT_SetDigitalOutput() (_TRISC2 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC3 GPIO Pin which has a custom name of SRV_POS to High
 * @pre      The RC3 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define SRV_POS_SetHigh()          (_LATC3 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC3 GPIO Pin which has a custom name of SRV_POS to Low
 * @pre      The RC3 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SRV_POS_SetLow()           (_LATC3 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RC3 GPIO Pin which has a custom name of SRV_POS
 * @pre      The RC3 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SRV_POS_Toggle()           (_LATC3 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RC3 GPIO Pin which has a custom name of SRV_POS
 * @param    none
 * @return   none  
 */
#define SRV_POS_GetValue()         _RC3

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC3 GPIO Pin which has a custom name of SRV_POS as Input
 * @param    none
 * @return   none  
 */
#define SRV_POS_SetDigitalInput()  (_TRISC3 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC3 GPIO Pin which has a custom name of SRV_POS as Output
 * @param    none
 * @return   none  
 */
#define SRV_POS_SetDigitalOutput() (_TRISC3 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC6 GPIO Pin which has a custom name of SRV_SENSE to High
 * @pre      The RC6 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define SRV_SENSE_SetHigh()          (_LATC6 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC6 GPIO Pin which has a custom name of SRV_SENSE to Low
 * @pre      The RC6 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SRV_SENSE_SetLow()           (_LATC6 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RC6 GPIO Pin which has a custom name of SRV_SENSE
 * @pre      The RC6 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SRV_SENSE_Toggle()           (_LATC6 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RC6 GPIO Pin which has a custom name of SRV_SENSE
 * @param    none
 * @return   none  
 */
#define SRV_SENSE_GetValue()         _RC6

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC6 GPIO Pin which has a custom name of SRV_SENSE as Input
 * @param    none
 * @return   none  
 */
#define SRV_SENSE_SetDigitalInput()  (_TRISC6 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC6 GPIO Pin which has a custom name of SRV_SENSE as Output
 * @param    none
 * @return   none  
 */
#define SRV_SENSE_SetDigitalOutput() (_TRISC6 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC8 GPIO Pin which has a custom name of BCD1 to High
 * @pre      The RC8 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define BCD1_SetHigh()          (_LATC8 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC8 GPIO Pin which has a custom name of BCD1 to Low
 * @pre      The RC8 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define BCD1_SetLow()           (_LATC8 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RC8 GPIO Pin which has a custom name of BCD1
 * @pre      The RC8 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define BCD1_Toggle()           (_LATC8 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RC8 GPIO Pin which has a custom name of BCD1
 * @param    none
 * @return   none  
 */
#define BCD1_GetValue()         _RC8

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC8 GPIO Pin which has a custom name of BCD1 as Input
 * @param    none
 * @return   none  
 */
#define BCD1_SetDigitalInput()  (_TRISC8 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC8 GPIO Pin which has a custom name of BCD1 as Output
 * @param    none
 * @return   none  
 */
#define BCD1_SetDigitalOutput() (_TRISC8 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC9 GPIO Pin which has a custom name of BCD2 to High
 * @pre      The RC9 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define BCD2_SetHigh()          (_LATC9 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC9 GPIO Pin which has a custom name of BCD2 to Low
 * @pre      The RC9 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define BCD2_SetLow()           (_LATC9 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RC9 GPIO Pin which has a custom name of BCD2
 * @pre      The RC9 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define BCD2_Toggle()           (_LATC9 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RC9 GPIO Pin which has a custom name of BCD2
 * @param    none
 * @return   none  
 */
#define BCD2_GetValue()         _RC9

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC9 GPIO Pin which has a custom name of BCD2 as Input
 * @param    none
 * @return   none  
 */
#define BCD2_SetDigitalInput()  (_TRISC9 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC9 GPIO Pin which has a custom name of BCD2 as Output
 * @param    none
 * @return   none  
 */
#define BCD2_SetDigitalOutput() (_TRISC9 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC13 GPIO Pin which has a custom name of EM_CS to High
 * @pre      The RC13 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define EM_CS_SetHigh()          (_LATC13 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC13 GPIO Pin which has a custom name of EM_CS to Low
 * @pre      The RC13 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define EM_CS_SetLow()           (_LATC13 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RC13 GPIO Pin which has a custom name of EM_CS
 * @pre      The RC13 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define EM_CS_Toggle()           (_LATC13 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RC13 GPIO Pin which has a custom name of EM_CS
 * @param    none
 * @return   none  
 */
#define EM_CS_GetValue()         _RC13

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC13 GPIO Pin which has a custom name of EM_CS as Input
 * @param    none
 * @return   none  
 */
#define EM_CS_SetDigitalInput()  (_TRISC13 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC13 GPIO Pin which has a custom name of EM_CS as Output
 * @param    none
 * @return   none  
 */
#define EM_CS_SetDigitalOutput() (_TRISC13 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD4 GPIO Pin which has a custom name of RCD_TEST to High
 * @pre      The RD4 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define RCD_TEST_SetHigh()          (_LATD4 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD4 GPIO Pin which has a custom name of RCD_TEST to Low
 * @pre      The RD4 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define RCD_TEST_SetLow()           (_LATD4 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD4 GPIO Pin which has a custom name of RCD_TEST
 * @pre      The RD4 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define RCD_TEST_Toggle()           (_LATD4 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD4 GPIO Pin which has a custom name of RCD_TEST
 * @param    none
 * @return   none  
 */
#define RCD_TEST_GetValue()         _RD4

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD4 GPIO Pin which has a custom name of RCD_TEST as Input
 * @param    none
 * @return   none  
 */
#define RCD_TEST_SetDigitalInput()  (_TRISD4 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD4 GPIO Pin which has a custom name of RCD_TEST as Output
 * @param    none
 * @return   none  
 */
#define RCD_TEST_SetDigitalOutput() (_TRISD4 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD5 GPIO Pin which has a custom name of SRV_ON to High
 * @pre      The RD5 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define SRV_ON_SetHigh()          (_LATD5 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD5 GPIO Pin which has a custom name of SRV_ON to Low
 * @pre      The RD5 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SRV_ON_SetLow()           (_LATD5 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD5 GPIO Pin which has a custom name of SRV_ON
 * @pre      The RD5 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SRV_ON_Toggle()           (_LATD5 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD5 GPIO Pin which has a custom name of SRV_ON
 * @param    none
 * @return   none  
 */
#define SRV_ON_GetValue()         _RD5

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD5 GPIO Pin which has a custom name of SRV_ON as Input
 * @param    none
 * @return   none  
 */
#define SRV_ON_SetDigitalInput()  (_TRISD5 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD5 GPIO Pin which has a custom name of SRV_ON as Output
 * @param    none
 * @return   none  
 */
#define SRV_ON_SetDigitalOutput() (_TRISD5 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD6 GPIO Pin which has a custom name of RCD_ERR to High
 * @pre      The RD6 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define RCD_ERR_SetHigh()          (_LATD6 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD6 GPIO Pin which has a custom name of RCD_ERR to Low
 * @pre      The RD6 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define RCD_ERR_SetLow()           (_LATD6 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD6 GPIO Pin which has a custom name of RCD_ERR
 * @pre      The RD6 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define RCD_ERR_Toggle()           (_LATD6 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD6 GPIO Pin which has a custom name of RCD_ERR
 * @param    none
 * @return   none  
 */
#define RCD_ERR_GetValue()         _RD6

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD6 GPIO Pin which has a custom name of RCD_ERR as Input
 * @param    none
 * @return   none  
 */
#define RCD_ERR_SetDigitalInput()  (_TRISD6 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD6 GPIO Pin which has a custom name of RCD_ERR as Output
 * @param    none
 * @return   none  
 */
#define RCD_ERR_SetDigitalOutput() (_TRISD6 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD7 GPIO Pin which has a custom name of RCD_F30MA_AC to High
 * @pre      The RD7 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define RCD_F30MA_AC_SetHigh()          (_LATD7 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD7 GPIO Pin which has a custom name of RCD_F30MA_AC to Low
 * @pre      The RD7 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define RCD_F30MA_AC_SetLow()           (_LATD7 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD7 GPIO Pin which has a custom name of RCD_F30MA_AC
 * @pre      The RD7 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define RCD_F30MA_AC_Toggle()           (_LATD7 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD7 GPIO Pin which has a custom name of RCD_F30MA_AC
 * @param    none
 * @return   none  
 */
#define RCD_F30MA_AC_GetValue()         _RD7

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD7 GPIO Pin which has a custom name of RCD_F30MA_AC as Input
 * @param    none
 * @return   none  
 */
#define RCD_F30MA_AC_SetDigitalInput()  (_TRISD7 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD7 GPIO Pin which has a custom name of RCD_F30MA_AC as Output
 * @param    none
 * @return   none  
 */
#define RCD_F30MA_AC_SetDigitalOutput() (_TRISD7 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD8 GPIO Pin which has a custom name of RCD_F6MA_DC to High
 * @pre      The RD8 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define RCD_F6MA_DC_SetHigh()          (_LATD8 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD8 GPIO Pin which has a custom name of RCD_F6MA_DC to Low
 * @pre      The RD8 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define RCD_F6MA_DC_SetLow()           (_LATD8 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD8 GPIO Pin which has a custom name of RCD_F6MA_DC
 * @pre      The RD8 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define RCD_F6MA_DC_Toggle()           (_LATD8 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD8 GPIO Pin which has a custom name of RCD_F6MA_DC
 * @param    none
 * @return   none  
 */
#define RCD_F6MA_DC_GetValue()         _RD8

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD8 GPIO Pin which has a custom name of RCD_F6MA_DC as Input
 * @param    none
 * @return   none  
 */
#define RCD_F6MA_DC_SetDigitalInput()  (_TRISD8 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD8 GPIO Pin which has a custom name of RCD_F6MA_DC as Output
 * @param    none
 * @return   none  
 */
#define RCD_F6MA_DC_SetDigitalOutput() (_TRISD8 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD9 GPIO Pin which has a custom name of FPGA_INT to High
 * @pre      The RD9 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define FPGA_INT_SetHigh()          (_LATD9 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD9 GPIO Pin which has a custom name of FPGA_INT to Low
 * @pre      The RD9 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define FPGA_INT_SetLow()           (_LATD9 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD9 GPIO Pin which has a custom name of FPGA_INT
 * @pre      The RD9 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define FPGA_INT_Toggle()           (_LATD9 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD9 GPIO Pin which has a custom name of FPGA_INT
 * @param    none
 * @return   none  
 */
#define FPGA_INT_GetValue()         _RD9

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD9 GPIO Pin which has a custom name of FPGA_INT as Input
 * @param    none
 * @return   none  
 */
#define FPGA_INT_SetDigitalInput()  (_TRISD9 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD9 GPIO Pin which has a custom name of FPGA_INT as Output
 * @param    none
 * @return   none  
 */
#define FPGA_INT_SetDigitalOutput() (_TRISD9 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD10 GPIO Pin which has a custom name of TEMP1 to High
 * @pre      The RD10 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define TEMP1_SetHigh()          (_LATD10 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD10 GPIO Pin which has a custom name of TEMP1 to Low
 * @pre      The RD10 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define TEMP1_SetLow()           (_LATD10 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD10 GPIO Pin which has a custom name of TEMP1
 * @pre      The RD10 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define TEMP1_Toggle()           (_LATD10 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD10 GPIO Pin which has a custom name of TEMP1
 * @param    none
 * @return   none  
 */
#define TEMP1_GetValue()         _RD10

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD10 GPIO Pin which has a custom name of TEMP1 as Input
 * @param    none
 * @return   none  
 */
#define TEMP1_SetDigitalInput()  (_TRISD10 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD10 GPIO Pin which has a custom name of TEMP1 as Output
 * @param    none
 * @return   none  
 */
#define TEMP1_SetDigitalOutput() (_TRISD10 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD12 GPIO Pin which has a custom name of FPGA_CDONE to High
 * @pre      The RD12 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define FPGA_CDONE_SetHigh()          (_LATD12 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD12 GPIO Pin which has a custom name of FPGA_CDONE to Low
 * @pre      The RD12 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define FPGA_CDONE_SetLow()           (_LATD12 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD12 GPIO Pin which has a custom name of FPGA_CDONE
 * @pre      The RD12 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define FPGA_CDONE_Toggle()           (_LATD12 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD12 GPIO Pin which has a custom name of FPGA_CDONE
 * @param    none
 * @return   none  
 */
#define FPGA_CDONE_GetValue()         _RD12

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD12 GPIO Pin which has a custom name of FPGA_CDONE as Input
 * @param    none
 * @return   none  
 */
#define FPGA_CDONE_SetDigitalInput()  (_TRISD12 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD12 GPIO Pin which has a custom name of FPGA_CDONE as Output
 * @param    none
 * @return   none  
 */
#define FPGA_CDONE_SetDigitalOutput() (_TRISD12 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD13 GPIO Pin which has a custom name of FPGA_CRST to High
 * @pre      The RD13 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define FPGA_CRST_SetHigh()          (_LATD13 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD13 GPIO Pin which has a custom name of FPGA_CRST to Low
 * @pre      The RD13 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define FPGA_CRST_SetLow()           (_LATD13 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD13 GPIO Pin which has a custom name of FPGA_CRST
 * @pre      The RD13 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define FPGA_CRST_Toggle()           (_LATD13 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD13 GPIO Pin which has a custom name of FPGA_CRST
 * @param    none
 * @return   none  
 */
#define FPGA_CRST_GetValue()         _RD13

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD13 GPIO Pin which has a custom name of FPGA_CRST as Input
 * @param    none
 * @return   none  
 */
#define FPGA_CRST_SetDigitalInput()  (_TRISD13 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD13 GPIO Pin which has a custom name of FPGA_CRST as Output
 * @param    none
 * @return   none  
 */
#define FPGA_CRST_SetDigitalOutput() (_TRISD13 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD14 GPIO Pin which has a custom name of FPGA_RST to High
 * @pre      The RD14 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define FPGA_RST_SetHigh()          (_LATD14 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD14 GPIO Pin which has a custom name of FPGA_RST to Low
 * @pre      The RD14 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define FPGA_RST_SetLow()           (_LATD14 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD14 GPIO Pin which has a custom name of FPGA_RST
 * @pre      The RD14 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define FPGA_RST_Toggle()           (_LATD14 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD14 GPIO Pin which has a custom name of FPGA_RST
 * @param    none
 * @return   none  
 */
#define FPGA_RST_GetValue()         _RD14

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD14 GPIO Pin which has a custom name of FPGA_RST as Input
 * @param    none
 * @return   none  
 */
#define FPGA_RST_SetDigitalInput()  (_TRISD14 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD14 GPIO Pin which has a custom name of FPGA_RST as Output
 * @param    none
 * @return   none  
 */
#define FPGA_RST_SetDigitalOutput() (_TRISD14 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Initializes the PINS module
 * @param    none
 * @return   none  
 */
void PINS_Initialize(void);

/**
 * @ingroup  pinsdriver
 * @brief    This function is callback for FPGA_INT Pin
 * @param    none
 * @return   none   
 */
void FPGA_INT_CallBack(void);


/**
 * @ingroup    pinsdriver
 * @brief      This function assigns a function pointer with a callback address
 * @param[in]  InterruptHandler - Address of the callback function 
 * @return     none  
 */
void FPGA_INT_SetInterruptHandler(void (* InterruptHandler)(void));


#endif
