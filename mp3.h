#ifndef MP3_H
#define MP3_H

#include <stdint.h>

// Function to send a command to the MP3 module
void MP3_SendCommand(uint8_t command, uint16_t parameter);

// Function to play MP3 corresponding to nearest building
void MP3_PlayBuilding(void);

// External function declaration (assumes it's defined elsewhere)
extern int findNearestBuildingIndex(int total);

void UART1_Init(void);
#endif // MP3_H
