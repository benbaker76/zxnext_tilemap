/*******************************************************************************
 * Word for tiles:
 * bits 15-12 : palette offset
 * bit 11 : x mirror
 * bit 10 : y mirror
 * bit 9 : rotate
 * bit 8 : ULA over tilemap (in 512 tile mode, bit 8 of the tile number)
 * bits 7-0 : tile number
 *
 * Read register
 *        ld      bc,0x243b
 *        out     (c),l
 *        inc     b
 *        in      l,(c)
 *        ld      h,0
 *
 * Tilemap address = port 0x6e
 * Tiles address = port 0x6f
 *
 * Tilemap config:
 * (R/W) 0x6B (107) => Tilemap Control
 *  bit 7    = 1 to enable the tilemap
 *  bit 6    = 0 for 40x32, 1 for 80x32
 *  bit 5    = 1 to eliminate the attribute entry in the tilemap
 *  bit 4    = palette select
 *  bits 3-2 = Reserved set to 0
 *  bit 1    = 1 to activate 512 tile mode
 *  bit 0    = 1 to force tilemap on top of ULA
 ******************************************************************************/

#ifndef _TILEMAP_H
#define _TILEMAP_H

#include <arch/zxn.h>
#include <stdint.h>
#include <stdbool.h>

// Tiles and maps must be in BANK_5 (0x4000)
#define POW2_ROUND(n, x)                (n + x - (n & (x - 1)))
#define TILE_BASE_OFF(n)                ((n - 0x4000) >> 8)
#define MAP_BASE_OFF(n)                 ((n - 0x4000) >> 8)

#define REG_CLIP_WINDOW_TILEMAP         0x1b

#define REG_TILEMAP_TRANSPARENCY_INDEX  0x4c

#define REG_TILEMAP_OFFSET_X_H          0x2f
#define REG_TILEMAP_OFFSET_X_L          0x30
#define REG_TILEMAP_OFFSET_Y            0x31

#define REG_TILEMAP_CONTROL             0x6b

#define REG_TILEMAP_DEFAULT_ATTRIBUTE   0x6c

#define REG_TILEMAP_BASE_ADDRESS        0x6e // bits 5-0 = Address of tilemap in BANK_5 ($40-$7f, $c0-$ff)
#define REG_TILEMAP_TILES_BASE_ADDRESS  0x6f // bits 5-0 = Address of tiles in BANK_5 ($40-$7f, $c0-$ff)

#define RPC_SELECT_TILEMAP_PALETTE_0    0x30
#define RPC_SELECT_TILEMAP_PALETTE_1    0x70

// REG_TILEMAP_CONTROL bits
#define TILEMAP_ENABLE                  0x80
#define TILEMAP_HIRES                   0x40
#define TILEMAP_NO_ATTRIBUTES           0x20
#define TILEMAP_PALETTE_SELECT          0x10
#define TILEMAP_TEXT_MODE               0x08
#define TILEMAP_ENABLE_512_TILES        0x02
#define TILEMAP_TILEMAP_OVER_ULA        0x01

// Attribute bits
// 7-4 = palette offset
#define TILEMAP_ATTRIBUTE_X_MIRROR      0x08
#define TILEMAP_ATTRIBUTE_Y_MIRROR      0x04
#define TILEMAP_ATTRIBUTE_ROTATE        0x02
#define TILEMAP_ULA_OVER_TILEMAP        0x01

void tilemap_init(uint16_t mapdata_offset, uint16_t tiledata_offset);
void tilemap_set_palette(const uint16_t *colors, uint8_t palette_select, uint16_t length, uint8_t palette_index);
void tilemap_reset_palette(uint8_t palette_select, uint8_t palette_index);
void tilemap_load_palette(const char *filename, uint8_t *buf_256);
void tilemap_load(const char *pal_filename, const char *tiles_filename, const char *map_filename, uint8_t *buf_256, uint16_t mapdata_offset, uint16_t tiledata_offset);
void tilemap_clear(uint8_t tile_id, uint16_t offset, uint16_t length, uint16_t mapdata_offset);
void tilemap_set_tile_id(uint8_t x, uint8_t y, uint8_t tile_id, uint16_t mapdata_offset);
uint8_t tilemap_get_tile_id(uint8_t x, uint8_t y, uint16_t mapdata_offset);
void *tilemap_get_tile_addr(uint8_t x, uint8_t y, uint16_t mapdata_offset);

#endif
