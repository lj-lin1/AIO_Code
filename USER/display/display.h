#ifndef BSP_DISPLAY_DISPLAY_H
#define BSP_DISPLAY_DISPLAY_H

#include "main.h"

#define BLACK  0
#define RED    1
#define GREEN  2
#define YELLOW 3
#define WHITE  4

extern u8 sourceBuf[28800];
extern u8 fontColor;

void lamp_scan(void);

void displaydataupdate(void);
void PowerOnDisplay(void);

#endif // !BSP_DISPLAY_DISPLAY_H