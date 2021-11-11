#ifndef _SPRITES_H
#define _SPRITES_H

#include <stdint.h>
#include <stdbool.h>

#define REG_SPRITE_ATTRIBUTES_INC  0x79
#define REG_SPRITE_SELECT  0x34

#define SPRITE_SLOT_MASK		0x3F
#define X_LSB_MASK				0x00FF
#define X_MSB_MASK				0x0100
#define X_MSB_SHIFT				8
#define Y_MSB_MASK				0x0100
#define Y_MSB_SHIFT				8
#define PALETTE_OFFSET_SHIFT	4
#define SPRITE_ENABLE_ATTRIB_4	(1 << 6)
#define SPRITE_VISIBLE			(1 << 7)

#define SPRITE_RELATIVE_ANCHOR	(1 << 5)
#define SPRITE_RELATIVE			(1 << 6)
#define SPRITE_RELATIVE_4BIT	(1 << 7)

#define X_LSB(x) (uint8_t) ((x) & X_LSB_MASK)
#define X_MSB(x) (uint8_t) (((x) & X_MSB_MASK) >> X_MSB_SHIFT)
#define X_EXT(x) (((uint16_t) (x)) + 32)
#define Y_EXT(y) ((y) + 32)
#define Y_MSB(x) (uint8_t) (((y) & Y_MSB_MASK) >> Y_MSB_SHIFT)

void sprite_set_display_palette(bool first_palette);
void sprite_set_rw_palette(bool first_palette);
void sprite_set_pattern(uint8_t sprite_slot, const void *sprite_pattern, bool pattern_4bit);
void sprite_set_attributes_rel(uint8_t sprite_index,
								uint8_t pattern_slot,
								uint16_t x,
								uint16_t y,
								uint8_t palette_offset,
								uint8_t sprite_flags,
								bool visible,
								bool anchor,
								bool pattern_4bit);
void sprite_set_palette(const uint16_t *colors, uint16_t length, uint8_t palette_index);
void sprites_clear();
void sprites_hide();
void sprite_update(uint8_t i, uint8_t x, uint8_t y);

#endif
