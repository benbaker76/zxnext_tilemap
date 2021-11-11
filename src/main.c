/*******************************************************************************
 * Ben Baker 2020
 *
 * A tilemap demo for Sinclair ZX Spectrum Next.
 ******************************************************************************/

#include <arch/zxn.h>
#include <arch/zxn/color.h>
#include <input.h>
#include <z80.h>
#include <intrinsic.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "globals.h"
#include "bank.h"
#include "ula.h"
#include "layer2.h"
#include "tiles.h"
#include "sprites.h"
#include "tilemap.h"
#include "dma.h"

#define CRT_6000	0

#if (CRT_6000 == 1)
#pragma output CRT_ORG_CODE = 0x6164
#else
#pragma output CRT_ORG_CODE = 0x8184
#endif
#pragma output REGISTER_SP = 0xC000
#pragma output CLIB_MALLOC_HEAP_SIZE = 0
#pragma output CLIB_STDIO_HEAP_SIZE = 0
#pragma output CLIB_FOPEN_MAX = -1
#pragma output CRT_ORG_BANK_17 = 0x0000
#pragma output CRT_ORG_BANK_18 = 0x0000

/*
 * Define IDE_FRIENDLY in your C IDE to disable Z88DK C extensions and avoid
 * parser errors/warnings in the IDE. Do NOT define IDE_FRIENDLY when compiling
 * the code with Z88DK.
 */
#ifdef IDE_FRIENDLY
#define __z88dk_fastcall
#define __preserves_regs(...)
#endif

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

static void init_hardware(void);

static void init_isr(void);

static void create_background(void);

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static int key = 0;

/*******************************************************************************
 * Functions
 ******************************************************************************/

static void hardware_init(void)
{
    // Put Z80 in 28 MHz turbo mode.
    ZXN_NEXTREG(REG_TURBO_MODE, RTM_28MHZ);

    // Disable RAM memory contention.
    ZXN_NEXTREGA(REG_PERIPHERAL_3, ZXN_READ_REG(REG_PERIPHERAL_3) | RP3_DISABLE_CONTENTION);
	
	ZXN_NEXTREG(REG_PALETTE_CONTROL, 0);
	ZXN_NEXTREG(REG_PALETTE_INDEX, 24);
	ZXN_NEXTREG(REG_PALETTE_VALUE_8, ZXN_RGB332_ZX_BRIGHT_MAGENTA);
	
	ZXN_NEXTREG(REG_TILEMAP_TRANSPARENCY_INDEX, 0);
	ZXN_NEXTREG(REG_GLOBAL_TRANSPARENCY_COLOR, ZXN_RGB332_ZX_BRIGHT_MAGENTA);
	ZXN_NEXTREG(REG_SPRITE_TRANSPARENCY_INDEX, ZXN_RGB332_ZX_BRIGHT_MAGENTA);
	
	ZXN_NEXTREGA(REG_SPRITE_LAYER_SYSTEM, RSLS_LAYER_PRIORITY_SUL | RSLS_SPRITES_OVER_BORDER | RSLS_SPRITES_VISIBLE);
	
	ZXN_NEXTREG(REG_LAYER_2_RAM_PAGE, PAGE_LAYER2);
	
	ZXN_NEXTREG(REG_CLIP_WINDOW_TILEMAP, 4);
	ZXN_NEXTREG(REG_CLIP_WINDOW_TILEMAP, 155);
	ZXN_NEXTREG(REG_CLIP_WINDOW_TILEMAP, 8);
	ZXN_NEXTREG(REG_CLIP_WINDOW_TILEMAP, 247);
	
	ZXN_NEXTREG(REG_ULA_CONTROL, 0x80);
	
#if (SCREEN_RES == RES_256x192)
	ZXN_NEXTREG(REG_LAYER_2_CONTROL, LAYER_2_256x192x8);
#elif (SCREEN_RES == RES_320x256)
	ZXN_NEXTREG(REG_LAYER_2_CONTROL, LAYER_2_320x256x8);
#elif (SCREEN_RES == RES_640x256)
	ZXN_NEXTREG(REG_LAYER_2_CONTROL, LAYER_2_640x256x4);
#endif
}

static void isr_init(void)
{
    // Set up IM2 interrupt service routine:
    // Put Z80 in IM2 mode with a 257-byte interrupt vector table located
    // at 0x6000 (before CRT_ORG_CODE) filled with 0x61 bytes. Install an
    // empty interrupt service routine at the interrupt service routine
    // entry at address 0x8181.

#if (CRT_6000 == 1)
	intrinsic_di();
	im2_init((void *) 0x6000);
	memset((void *) 0x6000, 0x61, 257);
	z80_bpoke(0x6161, 0xFB);
	z80_bpoke(0x6162, 0xED);
	z80_bpoke(0x6163, 0x4D);
	intrinsic_ei();
#else
	intrinsic_di();
	im2_init((void *) 0x8000);
	memset((void *) 0x8000, 0x81, 257);
	z80_bpoke(0x8181, 0xFB);
	z80_bpoke(0x8182, 0xED);
	z80_bpoke(0x8183, 0x4D);
	intrinsic_ei();
#endif
}

static void background_create(void)
{
    zx_border(INK_BLACK);
    zx_cls(BRIGHT | INK_BLACK | PAPER_WHITE);
}

static void tilemap_scroll_update(int16_t *tm_offset_x, int16_t *tm_offset_y, uint16_t *tm_map_offset_x, uint16_t *tm_map_offset_y, uint8_t tm_map_tiles_x, uint8_t tm_map_tiles_y)
{
	*tm_offset_x = CLAMP(*tm_offset_x, 0, (tm_map_tiles_x * TILE_X) - SCREEN_X);
	*tm_offset_y = CLAMP(*tm_offset_y, 0, (tm_map_tiles_y * TILE_X) - SCREEN_Y);

	*tm_map_offset_x = CLAMP(*tm_offset_x >> 3, 0, tm_map_tiles_x - SCREEN_TILES_X);
	*tm_map_offset_y = CLAMP(*tm_offset_y >> 3, 0, tm_map_tiles_y - SCREEN_TILES_Y);
		
	ZXN_WRITE_REG(REG_TILEMAP_OFFSET_X_L, *tm_offset_x & 7);
	ZXN_WRITE_REG(REG_TILEMAP_OFFSET_Y, *tm_offset_y & 7);
}

static void tilemap_update(uint16_t tm_map_offset_x, uint16_t tm_map_offset_y, uint8_t tm_map_tiles_x, uint8_t tm_map_tiles_y, uint16_t *tm_mapdata_offset, bool use_dma)
{
	for (int y = 0; y < SCREEN_TILES_Y; y++)
	{
		if (use_dma)
			dma_transfer(tm_mapdata_offset + (y * SCREEN_TILES_X), &tiles_nxm + (((tm_map_offset_y + y) * tm_map_tiles_x) + (tm_map_offset_x)), SCREEN_TILES_X * 2);
		else
			memcpy(tm_mapdata_offset + (y * SCREEN_TILES_X), &tiles_nxm + (((tm_map_offset_y + y) * tm_map_tiles_x) + (tm_map_offset_x)), SCREEN_TILES_X * 2);
	}
}

int main(void)
{
	int16_t tm_offset_x = 0, tm_offset_y = 0;
	uint16_t tm_map_offset_x = 0, tm_map_offset_y = 0;
	uint8_t tm_map_tiles_x = 48, tm_map_tiles_y = 48;
	uint16_t tm_tiledata_offset = 0x8000 - POW2_ROUND(((void *)&tiles_nxt_end - (void *)&tiles_nxt), 256);
	uint16_t tm_mapdata_offset = tm_tiledata_offset - POW2_ROUND(((void *)&tiles_nxm_end - (void *)&tiles_nxm), 256);

    hardware_init();
    isr_init();
	layer2_clear_screen(ZXN_RGB332_ZX_BRIGHT_MAGENTA);
    background_create();
	sprites_clear();
	tilemap_init(tm_mapdata_offset, tm_tiledata_offset);
	tilemap_clear(76, 0, SCREEN_TILES_X * SCREEN_TILES_Y * 2, tm_mapdata_offset);

	bank_set_16k(MMU_TILE_DATA, PAGE_TILE_DATA);
	bank_set_16k(MMU_TILES, PAGE_TILES);
	
	tilemap_set_palette((const uint16_t *)&tiles_nxp, 0, 16, 0);
	
	memcpy((void *)tm_tiledata_offset, (void *)&tiles_nxt, (void *)&tiles_nxt_end - (void *)&tiles_nxt);
	
	bank_set_16k(MMU_TILE_MAP, PAGE_TILE_MAP);
	
	tilemap_update(tm_map_offset_x, tm_map_offset_y, tm_map_tiles_x, tm_map_tiles_y, (uint16_t *)tm_mapdata_offset, false);

    while (true)
    {
		key = in_inkey();
		
		switch(key)
		{
		case 'a':
			tm_offset_x--;
			break;
		case 'd':
			tm_offset_x++;
			break;
		case 'w':
			tm_offset_y--;
			break;
		case 's':
			tm_offset_y++;
			break;
		}
		
        // Wait for vertical blanking interval.
        intrinsic_halt();

		tilemap_scroll_update(&tm_offset_x, &tm_offset_y, &tm_map_offset_x, &tm_map_offset_y, tm_map_tiles_x, tm_map_tiles_y);
		tilemap_update(tm_map_offset_x, tm_map_offset_y, tm_map_tiles_x, tm_map_tiles_y, (uint16_t *)tm_mapdata_offset, false);
    }

    // Trig a soft reset. The Next hardware registers and I/O ports will be reset by NextZXOS after a soft reset.
    ZXN_NEXTREG(REG_RESET, RR_SOFT_RESET);
    return 0;
}
