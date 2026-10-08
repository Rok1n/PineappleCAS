#ifndef TEST_GRAPHX_H
#define TEST_GRAPHX_H
#include <stdint.h>
uint8_t gfx_SetColor(uint8_t);
uint8_t gfx_SetTextFGColor(uint8_t);
void gfx_SetPixel(int, int);
void gfx_SetTextXY(int, int);
void gfx_PrintChar(char);
int gfx_GetCharWidth(char);
#endif
