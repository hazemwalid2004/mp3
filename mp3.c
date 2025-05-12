#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "string.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "UART0.h"
#include "uart5.h"
#include "uart3.h"
#include "lcd.h"
#include "globalVariables.h"
#include "mp3.h"
#include "gps.h"

void UART1_Init(void) {
    SYSCTL_RCGCUART_R |= 0x02;      // Enable UART1 clock
    SYSCTL_RCGCGPIO_R |= 0x02;      // Enable GPIOB clock
    while((SYSCTL_PRGPIO_R & 0x02) == 0);

    UART1_CTL_R &= ~UART_CTL_UARTEN;    // Disable UART1
    UART1_IBRD_R = 104;                 // 9600 baud rate (assuming 16 MHz clock)
    UART1_FBRD_R = 11;
    UART1_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);  // 8-bit, FIFO enabled
    UART1_CTL_R |= (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);  // Enable UART1

    GPIO_PORTB_AFSEL_R |= 0x03;         // Enable alternate functions for PB0 and PB1
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000011;
    GPIO_PORTB_DEN_R |= 0x03;           // Digital enable for PB0 and PB1
    GPIO_PORTB_AMSEL_R &= ~0x03;        // Disable analog on PB0 and PB1
}
void MP3_SendCommand(uint8_t command, uint16_t parameter) {
    uint8_t cmd[10];
    uint16_t checksum = 0xFFFF - (0xFF + 0x06 + 0x00 + command + (parameter >> 8) + (parameter & 0xFF)) + 1;
		int i;
    cmd[0] = 0x7E;             // Start byte
    cmd[1] = 0xFF;             // Version
    cmd[2] = 0x06;             // Length
    cmd[3] = command;          // Command
    cmd[4] = 0x00;             // No feedback
    cmd[5] = (parameter >> 8); // Parameter high byte
    cmd[6] = (parameter & 0xFF); // Parameter low byte
    cmd[7] = (checksum >> 8);  // Checksum high byte
    cmd[8] = (checksum & 0xFF); // Checksum low byte
    cmd[9] = 0xEF;             // End byte

    for( i = 0; i < 10; i++) {
        while((UART1_FR_R & UART_FR_TXFF) != 0);  // Wait until TX buffer is not full
        UART1_DR_R = cmd[i];
    }
}
void MP3_PlayBuilding(void) {
    // Assuming MP3 files are named 0001.mp3, 0002.mp3, ..., corresponding to index + 1
    MP3_SendCommand(0x03, findNearestBuildingIndex(numBuildings) + 1);  // 0x03 is the play command
}
