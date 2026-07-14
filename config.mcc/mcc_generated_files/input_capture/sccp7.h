/**
 * SCCP7-InputCapture Generated Driver Header File 
 * 
 * @file	  inputcapture_interface.h
 * 
 * @ingroup   inputcapturedriver
 * 
 * @brief 	  This is the generated driver header file for the SCCP7-InputCapture driver
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

#ifndef SCCP7_H
#define SCCP7_H

// Section: Included Files
#include <stdbool.h>
#include <stdint.h>
#include "input_capture_interface.h"

// Section: Data Type Definitions

/**
 * @ingroup  inputcapturedriver
 * @brief    Structure object of type INPUT_CAPTURE_INTERFACE with the custom name
 *           given by the user in the Melody Driver User interface. The default name 
 *           e.g. Input_Capture1 can be changed by the user in the INPUT_CAPTURE user interface. 
 *           This allows defining a structure with application specific name using 
 *           the 'Custom Name' field. Application specific name allows the API Portability.
*/
extern const struct INPUT_CAPTURE_INTERFACE RCD_PWM_READ;

/** 
  @ingroup  mccpdriver
  @brief    This macro is used to read the Input Capture timer frequency (in Hz) for 
            SCCP7 instance.
*/
#define SCCP7_CAPTURE_TIMER_FREQUENCY        100000000UL

/**
 * @ingroup  inputcapturedriver
 * @brief    This macro defines the Custom Name for \ref SCCP7_InputCapture_Initialize API
 */
#define RCD_PWM_READ_Initialize SCCP7_InputCapture_Initialize
/**
 * @ingroup  inputcapturedriver
 * @brief    This macro defines the Custom Name for \ref SCCP7_InputCapture_Deinitialize API
 */
#define RCD_PWM_READ_Deinitialize SCCP7_InputCapture_Deinitialize
/**
 * @ingroup  inputcapturedriver
 * @brief    This macro defines the Custom Name for \ref SCCP7_InputCapture_Start API
 */
#define RCD_PWM_READ_Start SCCP7_InputCapture_Start
/**
 * @ingroup  inputcapturedriver
 * @brief    This macro defines the Custom Name for \ref SCCP7_InputCapture_Stop API
 */
#define RCD_PWM_READ_Stop SCCP7_InputCapture_Stop
/**
 * @ingroup  inputcapturedriver
 * @brief    This macro defines the Custom Name for \ref SCCP7_InputCapture_Tasks API
 */
#define RCD_PWM_READ_Tasks SCCP7_InputCapture_Tasks
/**
 * @ingroup  inputcapturedriver
 * @brief    This macro defines the Custom Name for \ref SCCP7_InputCapture_DataRead API
 */
#define RCD_PWM_READ_DataRead SCCP7_InputCapture_DataRead
/**
 * @ingroup  inputcapturedriver
 * @brief    This macro defines the Custom Name for \ref SCCP7_InputCapture_HasBufferOverflowed API
 */
#define RCD_PWM_READ_HasBufferOverflowed SCCP7_InputCapture_HasBufferOverflowed
/**
 * @ingroup  inputcapturedriver
 * @brief    This macro defines the Custom Name for \ref SCCP7_InputCapture_IsBufferEmpty API
 */
#define RCD_PWM_READ_IsBufferEmpty SCCP7_InputCapture_IsBufferEmpty
/**
 * @ingroup  inputcapturedriver
 * @brief    This macro defines the Custom Name for \ref SCCP7_InputCapture_OverflowFlagClear API
 */
#define RCD_PWM_READ_OverflowFlagClear SCCP7_InputCapture_OverflowFlagClear
/**
 * @ingroup  inputcapturedriver
 * @brief    This macro defines the Custom Name for \ref SCCP7_InputCapture_CallbackRegister API
 */
#define RCD_PWM_READ_InputCapture_CallbackRegister SCCP7_InputCapture_CallbackRegister

// Section: Driver Interface Functions

/**
 * @ingroup  inputcapturedriver
 * @brief    Initializes the SCCP7-InputCapture module
 * @param    none
 * @return   none  
 */
void SCCP7_InputCapture_Initialize (void);

/**
 * @ingroup  inputcapturedriver
 * @brief    Deinitializes the SCCP7-InputCapture to POR values
 * @param    none
 * @return   none  
 */
void SCCP7_InputCapture_Deinitialize(void);

/**
 * @ingroup  inputcapturedriver
 * @brief    Starts the InputCapture operation
 * @pre      SCCP7_InputCapture_Initialize must be called
 * @param    none
 * @return   none  
 */
void SCCP7_InputCapture_Start(void);

/**
 * @ingroup  inputcapturedriver
 * @brief    Stops the InputCapture operation
 * @pre 	 SCCP7_InputCapture_Initialize must be called
 * @param    none
 * @return   none  
 */
void SCCP7_InputCapture_Stop(void);


/**
 * @ingroup  inputcapturedriver
 * @brief 	 This function is used to implement the tasks for polled implementations
 * @pre 	 \ref SCCP7_InputCapture_Initialize must be called
 * @param    none
 * @return   none  
 */
void SCCP7_InputCapture_Tasks(void);

/**
 * @ingroup   inputcapturedriver
 * @brief      This function can be used to override default callback and to 
 *             define custom callback for SCCP7 InputCapture event.
 * @param[in]  handler - Address of the callback function  
 * @return     none    
 */
void SCCP7_InputCapture_CallbackRegister(void (*handler)(void));

/**
 * @ingroup  inputcapturedriver
 * @brief    This is the default callback with weak attribute. The user can override 
 *           and implement the default callback without weak attribute or can register 
 *           a custom callback function using  SCCP7_InputCaptureCallbackRegister.
 * @param    none
 * @return   none 
 */
void SCCP7_InputCapture_Callback(void);

/**
 * @ingroup  inputcapturedriver
 * @brief 	 Reads the captured value
 * @pre 	 \ref SCCP7_InputCapture_Initialize must be called.
 * @param    none
 * @return   Returns the captured value
 */
uint32_t SCCP7_InputCapture_DataRead(void);


/**
 * @ingroup  inputcapturedriver
 * @brief 	 Returns the buffer overflow status
 * @pre 	 \ref SCCP7_InputCapture_Initialize must be called
 * @param    none
 * @return   true  - input capture buffer has overflowed
 * @return   false - input capture buffer has not overflowed
 */
bool SCCP7_InputCapture_HasBufferOverflowed(void);


/**
 * @ingroup  inputcapturedriver
 * @brief	 Returns the buffer empty status
 * @pre 	 \ref SCCP7_InputCapture_Initialize must be called
 * @param    none
 * @return   true   -  input capture buffer is empty
 * @return   false  -  input capture buffer is not empty
 */
bool SCCP7_InputCapture_IsBufferEmpty(void);

/**
 * @ingroup  inputcapturedriver
 * @brief	 Clears the buffer overflow status flag
 * @pre 	 \ref SCCP7_InputCapture_Initialize must be called
 * @param    none
 * @return   none      
 */
void SCCP7_InputCapture_OverflowFlagClear(void);

#endif //SCCP7_H

/**
 End of File
*/


