/**
 * @file main.c
 * @author Jesus Minjares (https://github.com/jminjares4)
 * @brief Bluetooth header file
 * @version 0.1
 * @date 04-09-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_


#define BUFFER_SIZE 32 // set buffer size

void set3Mhz(void);
void UART0_init(void);
void UART2_init(void);
void sendUART0(char *message);
void sendUART2(char *message);
void sendCharUART0(char c);
void sendCharUART2(char c);

#endif /* BLUETOOTH_H_ */
