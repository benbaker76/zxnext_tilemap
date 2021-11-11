#include "sprites.h"
#include <arch/zxn.h>
#include <intrinsic.h>
#include <string.h>
#include "globals.h"
#include "bank.h"

void sprite_set_display_palette(bool first_palette)
{
	IO_NEXTREG_REG = REG_PALETTE_CONTROL;
	IO_NEXTREG_DAT = (IO_NEXTREG_DAT & 0xF7) | (first_palette ? 0x00 : 0x08);
}

void sprite_set_rw_palette(bool first_palette)
{
	IO_NEXTREG_REG = REG_PALETTE_CONTROL;
	IO_NEXTREG_DAT = (IO_NEXTREG_DAT & 0x8F) | (first_palette ? 0x20 : 0x60);
}

void sprite_set_pattern(uint8_t sprite_slot, const void *sprite_pattern, bool pattern_4bit)
{
	if (pattern_4bit)
	{
		IO_SPRITE_SLOT = (sprite_slot >> 1) | (sprite_slot << 7);
		intrinsic_outi((void *) sprite_pattern, __IO_SPRITE_PATTERN, 128);
	}
	else
	{
		IO_SPRITE_SLOT = sprite_slot & SPRITE_SLOT_MASK;
		intrinsic_outi((void *) sprite_pattern, __IO_SPRITE_PATTERN, 256);
	}
}

void sprite_set_attributes_rel(uint8_t sprite_index,
								uint8_t pattern_slot,
								uint16_t x,
								uint16_t y,
								uint8_t palette_offset,
								uint8_t sprite_flags,
								bool visible,
								bool anchor,
								bool pattern_4bit)
{
	const uint8_t N = (pattern_4bit ? pattern_slot >> 1 : pattern_slot) & SPRITE_SLOT_MASK;
	const uint8_t N6 = pattern_4bit ? pattern_slot & 1 : 0;
	
	IO_SPRITE_SLOT = sprite_index;
	IO_SPRITE_ATTRIBUTE = X_LSB(x);
	IO_SPRITE_ATTRIBUTE = y;
	IO_SPRITE_ATTRIBUTE = (palette_offset << PALETTE_OFFSET_SHIFT) | X_MSB(x) | sprite_flags;
	IO_SPRITE_ATTRIBUTE = (visible ? SPRITE_VISIBLE : 0) | SPRITE_ENABLE_ATTRIB_4 | N;

	uint8_t attrib4 = 0;
	
	if (anchor)
	{
		attrib4 |= (pattern_4bit ? (1 << 7) : 0); // 4-bit
		attrib4 |= (N6 << 6); // N6 pattern bit
		attrib4 |= (1 << 5); // unified
		attrib4 |= (y >> 8) & 1; // 9th bit of y coord
	}
	else // unified
	{
		attrib4 |= (1 << 6);
		attrib4 |= (N6 << 5); // N6 pattern bit
		attrib4 |= 1; // pattern number is relative to the anchor’s
	}
	
	IO_SPRITE_ATTRIBUTE = attrib4;
}

void sprite_set_palette(const uint16_t *colors, uint16_t length, uint8_t palette_index)
{
	uint8_t *color_bytes = (uint8_t *) colors;
	uint16_t i;

	if ((colors == NULL) || (length == 0))
	{
		//return;
	}

	if (palette_index + length > 256)
	{
		length = 256 - palette_index;
	}

	IO_NEXTREG_REG = REG_PALETTE_INDEX;
	IO_NEXTREG_DAT = palette_index;

	IO_NEXTREG_REG = REG_PALETTE_VALUE_16;
	for (i = 0; i < (length << 1); i++)
	{
		IO_NEXTREG_DAT = color_bytes[i];
	}
}

void sprites_clear()
{
	ZXN_NEXTREG(REG_SPRITE_SELECT, 0);
	
	IO_NEXTREG_REG = REG_SPRITE_ATTRIBUTES_INC;

	for (unsigned int i = 0; i != 128; ++i)
		IO_NEXTREG_DAT = 0;
}

void sprites_hide()
{
	for (uint8_t i = 0; i < 32; i++)
	{
		uint8_t sprite_index = i * 4;
		sprite_set_attributes_rel(sprite_index, sprite_index, 0, 0, 0, 0, false, true, true);
	}
}

void sprite_update(uint8_t i, uint8_t x, uint8_t y)
{
	uint8_t sprite_index = i * 4;
	sprite_set_attributes_rel(sprite_index, sprite_index, x, y, 0, 0, true, true, true);
}
