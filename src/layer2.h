#ifndef _LAYER2_H
#define _LAYER2_H

#include <stdint.h>
#include <stdbool.h>

#define LAYER_2_256x192x8	0x00
#define LAYER_2_320x256x8	0x10
#define LAYER_2_640x256x4	0x20

#define REG_LAYER_2_OFFSET_X_H  0x71
#define REG_LAYER_2_CONTROL  112

void layer2_draw_char(uint16_t x, uint16_t y, char ch, uint8_t color);
void layer2_draw_text(uint8_t row, uint8_t column, const char *text, uint8_t color);
void layer2_set_screen(char *p_image, uint8_t image_bank, uint8_t image_page);
void layer2_clear_screen(uint8_t value);

#endif
