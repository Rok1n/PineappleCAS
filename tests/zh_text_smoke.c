/* Host-side smoke tests for the GraphX bitmap localization renderer. */
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

static uint8_t current_color = 18;
static uint8_t text_color = 0;
static int pixels_drawn = 0;
static int invalid_pixels = 0;

uint8_t gfx_SetColor(uint8_t color) {
    uint8_t old = current_color;
    current_color = color;
    return old;
}
uint8_t gfx_SetTextFGColor(uint8_t color) {
    uint8_t old = text_color;
    text_color = color;
    return old;
}
void gfx_SetPixel(int x, int y) {
    if (x < 0 || x >= 320 || y < 0 || y >= 240) ++invalid_pixels;
    ++pixels_drawn;
}
void gfx_SetTextXY(int x, int y) {
    (void)x;
    (void)y;
}
void gfx_PrintChar(char character) {
    (void)character;
}
int gfx_GetCharWidth(char character) {
    (void)character;
    return 6;
}

#include "../src/calc/zh_text.h"

int main(void) {
    assert(zh_string_width("输入") == 24);
    assert(zh_string_width("求导") == 24);
    assert(zh_string_width("Ans") == 18);
    assert(zh_string_width("输入 Ans") == 48);
    assert(zh_find(0x8F93) != 0);
    assert(zh_find(0x4E2D) != 0);
    assert(zh_find(0x8000) == 0);

    zh_print_xy("输入 Ans", 50, 60, 0xFF);
    assert(pixels_drawn > 0);
    assert(invalid_pixels == 0);
    assert(current_color == 18);
    assert(text_color == 0xFF);

    puts("PASS: bitmap font renderer smoke tests");
    return 0;
}
