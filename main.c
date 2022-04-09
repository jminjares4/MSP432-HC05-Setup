/**
 *  @file main.c
 *  @author Jesus Minjares (https://github.com/jminjares4)
 *  @brief main
 *  @version 0.1
 *  @date 04-09-2022
 *
 * App: The purpose of this code is to reconfigure the HC05/HC06
 *      bluetooth modules. AT+COMMANDS are use to update and get HC05/HC06
 *      bluetooth modules. This repository will facilitate the use of HC05/HC06
 *      configuration.
 *
 *      S: State
 *      RX: Receiver
 *      TX: Transmitter
 *      VCC: 3.3-5v
 *      GND: 0v
 *      EN: Set on high, for AT Mode
 *                                                -----------------
 *               MSP432P401x                     |      HC-05      |
 *            -----------------                   S RX TX VCC GND EN
 *           |          0V(GND)|-->HC-05 GND
 *           |                 |
 *           |RST         VCC  |-->HC-05 VCC
 *           |                 |
 *           |         P3.2(RX)|-->HC-05 TX
 *           |         P3.3(TX)|-->HC-05 RX
 *           |                 |
 *           |_________________|
 *
 *
 *
 *
 *  @copyright Copyright (c) 2022
*/
#include "msp.h"
#include "string.h"
#include "bluetooth.h"

char buffer[BUFFER_SIZE];         // UART2 buffer
uint8_t bufferIndex = 0;          // UART2 buffer index

char keyboardBuffer[BUFFER_SIZE]; // UART0 buffer
uint8_t keyboardIndex = 0;        // UART0 buffer index

void main(void)
 {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    /*Set DCO Clock @ 3Mhz*/
    set3Mhz();

    /*Initialize UART 0 @ 9600 */
    UART0_init();
    /*Initialize UART 2 @ 9600 */
    UART2_init();

    /*Enable global interrupts*/
    __enable_irq(); //enable interrupts

    while(1){
        /*Set MCU to low power mode*/
        SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
        __sleep();
    }
}

void EUSCIA0_IRQHandler(void){
    /*Check RX flag*/
    if(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG){
          keyboardBuffer[keyboardIndex] = EUSCI_A0->RXBUF; //store character

          sendCharUART0(keyboardBuffer[keyboardIndex]); // send latest character recieved

          /*Check carriage return*/
          if(keyboardBuffer[keyboardIndex++] == '\r'){
              sendUART0("\r\n"); //transmit carriage return and new line
              keyboardBuffer[keyboardIndex] = '\n'; // set the last character to new line
              sendUART2(keyboardBuffer); // transmit buffer
              //clear buffer
              memset(keyboardBuffer, 0, sizeof(keyboardBuffer));
              //reset index
              keyboardIndex = 0;
          }
       }
}
void EUSCIA2_IRQHandler(void){
    /*Check RX flag*/
    if(EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG){
        buffer[bufferIndex] = EUSCI_A2->RXBUF; //store character
        /*Check carriage return*/
        if(buffer[bufferIndex++] == '\r'){
               buffer[bufferIndex] = '\n'; // set the last character to new line
               sendUART0(buffer); //transmit buffer
               //clear buffer
               memset(buffer, 0, sizeof(buffer));
               //reset index
               bufferIndex = 0;
        }
    }
}
