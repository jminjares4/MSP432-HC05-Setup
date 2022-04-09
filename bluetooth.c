/**
 * @file main.c
 * @author Jesus Minjares (https://github.com/jminjares4)
 * @brief Bluetooth source file
 * @version 0.1
 * @date 04-09-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "msp.h"
#include "string.h"
#include "bluetooth.h"

/**
 * @brief   Initialize system clock to 3Mhz
 * @param   None
 * @note    This function will set the DCO clock to be set @ 3Mhz
 * @return  None
 */
void set3Mhz(void){
    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_1;           // Set DCO to 3MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 |CS_CTL1_SELM_3; // Select ACLK = REFO// MCLK = DCO // SMCLK = DCO
    CS->KEY = 0; //lock CS module
}

/**
 * @brief   Initialize UART0 for the onboard serial port
 * @param   None
 * @note    This function will set UART0 @ 9600 baudrate
 * @return  None
 */
void UART0_init(void){
    // Configure UART pins
      P1->SEL0 |= BIT2 | BIT3;                // set UART pin as secondary function
      P1->SEL1 &= ~(BIT2+BIT3);
      // Configure UART
      EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
      EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | EUSCI_A_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
      EUSCI_A0->BRW = 19;                     // 3MHz/16/9600 = 19.53125, 19
      EUSCI_A0->MCTLW = (9 << EUSCI_A_MCTLW_BRF_OFS) |EUSCI_A_MCTLW_OS16; // 19.53125 - 19 = .53125*16 = 8.5, round up
      EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
      EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
      EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt
      // Enable eUSCIA0 interrupt in NVIC module
      NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
}

/**
 * @brief   Initialize UART2 for the HC05/HC06 Bluetooth module
 * @param   None
 * @note     This function will set UART0 @ 9600 baudrate
 * @return  None
 */
void UART2_init(void){
    // Configure UART pins
    P3->SEL0 |= BIT2 | BIT3;                // set UART pin as secondary function
    P3->SEL1 &= ~(BIT2+BIT3);
    // Configure UART
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
    EUSCI_A2->BRW = 4; // 3Mhz/(38400) = 78.125 / 16 = 4.8828125
    EUSCI_A2->MCTLW = (15 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16; //(4.8828125-4)*16 = 14.125 -> 15

    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A2->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A2 RX interrupt
    // Enable eUSCIA2 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
}

/**
 * @brief   Send message through UART0
 * @param   string to be transimitted
 * @note    This function will transmit the string to UART0
 * @return  None
 */
void sendUART0(char *message){
    int i; //create variable
    for(i = 0; i < strlen(message); i++){ //iterate over length of the string
         while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)); //check TX flag
                EUSCI_A0->TXBUF = message[i]; //set char to TXBUF
    }
    return;
}

/**
 * @brief   Send message through UART2
 * @param   string to be transimitted
 * @note    This function will transmit the string to UART2
 * @return  None
 */
void sendUART2(char *message){
    int i; //create variable
    for(i = 0; i < strlen(message); i++){ //iterate over length of the string
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG)); //check TX flag
        EUSCI_A2->TXBUF = message[i]; //set char to TXBUF
    }
    return;
}

/**
 * @brief   Send character through UART0
 * @param   character to be transimitted
 * @note    This function will transmit the character to UART0
 * @return  None
 */
void sendCharUART0(char c){
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)); //check TX flag
    EUSCI_A0->TXBUF = c; //set char to TXBUF
    return;
}

/**
 * @brief   Send character through UART2
 * @param   character to be transimitted
 * @note    This function will transmit the character to UART2
 * @return  None
 */
void sendCharUART2(char c){
    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG)); //check TX flag
    EUSCI_A2->TXBUF = c; //set char to TXBUF
    return;
}
