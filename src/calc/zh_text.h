/* Minimal UTF-8/bitmap text renderer for the TI-84 Plus CE GUI.
 * Chinese glyphs are bundled at build time; TI-OS fonts are unaffected.
 * ASCII characters continue to use the native GraphX font.
 */
#ifndef PCAS_ZH_TEXT_H
#define PCAS_ZH_TEXT_H

#include <stdint.h>
#include <graphx.h>
#include "zh_font.h"

static const zh_glyph_t *zh_find(uint16_t code) {
    unsigned lo = 0, hi = (unsigned)ZH_GLYPH_COUNT;
    while (lo < hi) {
        unsigned middle = lo + (hi - lo) / 2;
        if (zh_glyphs[middle].unicode == code) return &zh_glyphs[middle];
        if (zh_glyphs[middle].unicode < code) lo = middle + 1;
        else hi = middle;
    }
    return 0;
}

/* Decode the ASCII and 3-byte BMP characters used in GUI labels only. */
static const char *zh_next(const char *s, uint16_t *code) {
    uint8_t a = (uint8_t)s[0];
    if (a < 0x80) {
        *code = a;
        return s + 1;
    }
    if ((a & 0xF0) == 0xE0 && s[1] && s[2] &&
        ((uint8_t)s[1] & 0xC0) == 0x80 &&
        ((uint8_t)s[2] & 0xC0) == 0x80) {
        *code = (uint16_t)(((a & 0x0F) << 12) |
                 (((uint8_t)s[1] & 0x3F) << 6) | ((uint8_t)s[2] & 0x3F));
        return s + 3;
    }
    *code = '?';
    return s + 1;
}

static unsigned zh_string_width(const char *s) {
    unsigned w = 0;
    while (*s) {
        uint16_t code;
        s = zh_next(s, &code);
        w += code < 128 ? gfx_GetCharWidth((char)code) : ZH_GLYPH_W;
    }
    return w;
}

static void zh_print_xy(const char *s, int x, int y, uint8_t color) {
    uint8_t old_color = gfx_SetColor(color);
    gfx_SetTextFGColor(color);
    while (*s) {
        const zh_glyph_t *glyph;
        uint16_t code;
        unsigned row, col;
        s = zh_next(s, &code);
        glyph = zh_find(code);
        if (glyph) {
            /* Offset the 12px glyph to the center of GraphX's 8px line. */
            for (row = 0; row < ZH_GLYPH_H; ++row) {
                for (col = 0; col < ZH_GLYPH_W; ++col) {
                    if (glyph->rows[row] & (1u << (ZH_GLYPH_W - col - 1))) {
                        gfx_SetPixel(x + (int)col, y - 2 + (int)row);
                    }
                }
            }
            x += ZH_GLYPH_W;
        } else if (code < 128) {
            gfx_SetTextXY(x, y);
            gfx_PrintChar((char)code);
            x += gfx_GetCharWidth((char)code);
        } else {
            gfx_SetTextXY(x, y);
            gfx_PrintChar('?');
            x += ZH_GLYPH_W;
        }
    }
    gfx_SetColor(old_color);
}

#endif
