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
 * @brief    Sets the RB12 GPIO Pin which has a custom name of LED_G to High
 * @pre      The RB12 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define LED_G_SetHigh()          (_LATB12 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB12 GPIO Pin which has a custom name of LED_G to Low
 * @pre      The RB12 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED_G_SetLow()           (_LATB12 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB12 GPIO Pin which has a custom name of LED_G
 * @pre      The RB12 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED_G_Toggle()           (_LATB12 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB12 GPIO Pin which has a custom name of LED_G
 * @param    none
 * @return   none  
 */
#define LED_G_GetValue()         _RB12

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB12 GPIO Pin which has a custom name of LED_G as Input
 * @param    none
 * @return   none  
 */
#define LED_G_SetDigitalInput()  (_TRISB12 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB12 GPIO Pin which has a custom name of LED_G as Output
 * @param    none
 * @return   none  
 */
#define LED_G_SetDigitalOutput() (_TRISB12 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB13 GPIO Pin which has a custom name of LED_W to High
 * @pre      The RB13 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define LED_W_SetHigh()          (_LATB13 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB13 GPIO Pin which has a custom name of LED_W to Low
 * @pre      The RB13 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED_W_SetLow()           (_LATB13 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB13 GPIO Pin which has a custom name of LED_W
 * @pre      The RB13 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED_W_Toggle()           (_LATB13 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB13 GPIO Pin which has a custom name of LED_W
 * @param    none
 * @return   none  
 */
#define LED_W_GetValue()         _RB13

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB13 GPIO Pin which has a custom name of LED_W as Input
 * @param    none
 * @return   none  
 */
#define LED_W_SetDigitalInput()  (_TRISB13 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB13 GPIO Pin which has a custom name of LED_W as Output
 * @param    none
 * @return   none  
 */
#define LED_W_SetDigitalOutput() (_TRISB13 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB14 GPIO Pin which has a custom name of LED_R to High
 * @pre      The RB14 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define LED_R_SetHigh()          (_LATB14 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB14 GPIO Pin which has a custom name of LED_R to Low
 * @pre      The RB14 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED_R_SetLow()           (_LATB14 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB14 GPIO Pin which has a custom name of LED_R
 * @pre      The RB14 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED_R_Toggle()           (_LATB14 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB14 GPIO Pin which has a custom name of LED_R
 * @param    none
 * @return   none  
 */
#define LED_R_GetValue()         _RB14

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB14 GPIO Pin which has a custom name of LED_R as Input
 * @param    none
 * @return   none  
 */
#define LED_R_SetDigitalInput()  (_TRISB14 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB14 GPIO Pin which has a custom name of LED_R as Output
 * @param    none
 * @return   none  
 */
#define LED_R_SetDigitalOutput() (_TRISB14 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB15 GPIO Pin which has a custom name of LED_B to High
 * @pre      The RB15 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define LED_B_SetHigh()          (_LATB15 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB15 GPIO Pin which has a custom name of LED_B to Low
 * @pre      The RB15 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED_B_SetLow()           (_LATB15 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB15 GPIO Pin which has a custom name of LED_B
 * @pre      The RB15 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED_B_Toggle()           (_LATB15 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB15 GPIO Pin which has a custom name of LED_B
 * @param    none
 * @return   none  
 */
#define LED_B_GetValue()         _RB15

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB15 GPIO Pin which has a custom name of LED_B as Input
 * @param    none
 * @return   none  
 */
#define LED_B_SetDigitalInput()  (_TRISB15 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB15 GPIO Pin which has a custom name of LED_B as Output
 * @param    none
 * @return   none  
 */
#define LED_B_SetDigitalOutput() (_TRISB15 = 0)

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
 * @brief    Sets the RD0 GPIO Pin which has a custom name of SRV_PWM to High
 * @pre      The RD0 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define SRV_PWM_SetHigh()          (_LATD0 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD0 GPIO Pin which has a custom name of SRV_PWM to Low
 * @pre      The RD0 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SRV_PWM_SetLow()           (_LATD0 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD0 GPIO Pin which has a custom name of SRV_PWM
 * @pre      The RD0 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SRV_PWM_Toggle()           (_LATD0 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD0 GPIO Pin which has a custom name of SRV_PWM
 * @param    none
 * @return   none  
 */
#define SRV_PWM_GetValue()         _RD0

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD0 GPIO Pin which has a custom name of SRV_PWM as Input
 * @param    none
 * @return   none  
 */
#define SRV_PWM_SetDigitalInput()  (_TRISD0 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD0 GPIO Pin which has a custom name of SRV_PWM as Output
 * @param    none
 * @return   none  
 */
#define SRV_PWM_SetDigitalOutput() (_TRISD0 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD1 GPIO Pin which has a custom name of LED_SOCKET to High
 * @pre      The RD1 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define LED_SOCKET_SetHigh()          (_LATD1 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD1 GPIO Pin which has a custom name of LED_SOCKET to Low
 * @pre      The RD1 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED_SOCKET_SetLow()           (_LATD1 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD1 GPIO Pin which has a custom name of LED_SOCKET
 * @pre      The RD1 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED_SOCKET_Toggle()           (_LATD1 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD1 GPIO Pin which has a custom name of LED_SOCKET
 * @param    none
 * @return   none  
 */
#define LED_SOCKET_GetValue()         _RD1

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD1 GPIO Pin which has a custom name of LED_SOCKET as Input
 * @param    none
 * @return   none  
 */
#define LED_SOCKET_SetDigitalInput()  (_TRISD1 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD1 GPIO Pin which has a custom name of LED_SOCKET as Output
 * @param    none
 * @return   none  
 */
#define LED_SOCKET_SetDigitalOutput() (_TRISD1 = 0)

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
 * @brief    Initializes the PINS module
 * @param    none
 * @return   none  
 */
void PINS_Initialize(void);



#endif
