/*******************************************************************************
 * Ben Baker 2020
 *
 * Implementation of get_sprite_system_state() in zxnext_tilemap.h.
 ******************************************************************************/

#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <z80.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "bank.h"
#include "tilemap.h"
#include "globals.h"
#include "dma.h"

void tilemap_init(uint16_t mapdata_offset, uint16_t tiledata_offset)
{
	ZXN_WRITE_REG(REG_TILEMAP_CONTROL, TILEMAP_ENABLE | TILEMAP_TILEMAP_OVER_ULA | TILEMAP_ENABLE_512_TILES);
	ZXN_WRITE_REG(REG_TILEMAP_DEFAULT_ATTRIBUTE, 0);
	
	ZXN_WRITE_REG(REG_TILEMAP_BASE_ADDRESS, MAP_BASE_OFF(mapdata_offset));
	ZXN_WRITE_REG(REG_TILEMAP_TILES_BASE_ADDRESS, TILE_BASE_OFF(tiledata_offset));
	
	ZXN_WRITE_REG(REG_TILEMAP_OFFSET_X_H, 0); // 1
	ZXN_WRITE_REG(REG_TILEMAP_OFFSET_X_L, 0); // 64
	ZXN_WRITE_REG(REG_TILEMAP_OFFSET_Y, 0);
}

void tilemap_set_palette(const uint16_t *colors, uint8_t palette_select, uint16_t length, uint8_t palette_index)
{
    uint8_t *color_bytes = (uint8_t *) colors;
    uint16_t i;

    if ((colors == NULL) || (length == 0))
    {
        return;
    }

    if (palette_index + length > 256)
    {
        length = 256 - palette_index;
    }
	
	if (palette_select == 0)
	{
		ZXN_NEXTREG(REG_PALETTE_CONTROL, RPC_SELECT_TILEMAP_PALETTE_0);
	}
	else
	{
		ZXN_NEXTREG(REG_PALETTE_CONTROL, RPC_SELECT_TILEMAP_PALETTE_1);
	}

    IO_NEXTREG_REG = REG_PALETTE_INDEX;
    IO_NEXTREG_DAT = palette_index;

    IO_NEXTREG_REG = REG_PALETTE_VALUE_16;
    for (i = 0; i < (length << 1); i++)
    {
        IO_NEXTREG_DAT = color_bytes[i];
    }
}

void tilemap_reset_palette(uint8_t palette_select, uint8_t palette_index)
{
    uint16_t i;
	
	if (palette_select == 0)
	{
		ZXN_NEXTREG(REG_PALETTE_CONTROL, RPC_SELECT_TILEMAP_PALETTE_0);
	}
	else
	{
		ZXN_NEXTREG(REG_PALETTE_CONTROL, RPC_SELECT_TILEMAP_PALETTE_1);
	}
	
    IO_NEXTREG_REG = REG_PALETTE_INDEX;
    IO_NEXTREG_DAT = palette_index;

    IO_NEXTREG_REG = REG_PALETTE_VALUE_16;
    for (i = 0; i < 256; i++)
    {
        IO_NEXTREG_DAT = (uint8_t) i;
    }
}

void tilemap_load_palette(const char *filename, uint8_t *buf_256)
{
    uint8_t filehandle;
    errno = 0;

    if (filename == NULL)
		return;
	
	filehandle = esxdos_f_open(filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
	if (errno)
	{
		return;
	}

	esxdos_f_read(filehandle, buf_256, 256);
	if (errno)
	{
		goto end;
	}
	tilemap_set_palette((uint16_t *) buf_256, 0, 128, 0);

	esxdos_f_read(filehandle, buf_256, 256);
	if (errno)
	{
		goto end;
	}
	tilemap_set_palette((uint16_t *) buf_256, 0, 128, 128);
	
end:
	
	esxdos_f_close(filehandle);
}

void tilemap_load(const char *pal_filename, const char *tiles_filename, const char *map_filename, uint8_t *buf_256, uint16_t mapdata_offset, uint16_t tiledata_offset)
{
    uint8_t filehandle;
    uint8_t old_page;
    uint8_t i;

	tilemap_load_palette(pal_filename, buf_256);

    errno = 0;

	filehandle = esxdos_f_open(tiles_filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
	
    old_page = ZXN_READ_REG(REG_MMU2);
	
    for (i = 0; i < 2; i++)
    {
		ZXN_WRITE_REG(REG_MMU2 + i, 5 + i);
        esxdos_f_read(filehandle, (uint8_t *)tiledata_offset + (i * BANKSIZE_8K), BANKSIZE_8K);
        if (errno)
        {
            break;
        }
    }

    esxdos_f_close(filehandle);

    errno = 0;

	filehandle = esxdos_f_open(map_filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
	
	ZXN_WRITE_REG(REG_MMU3, 6);
	
	esxdos_f_read(filehandle, (uint8_t *)mapdata_offset, BANKSIZE_8K);

    esxdos_f_close(filehandle);

	//ZXN_WRITE_MMU2(old_page);
    //ZXN_WRITE_MMU3(old_page + 1);
}

void tilemap_clear(uint8_t tile_id, uint16_t offset, uint16_t length, uint16_t mapdata_offset)
{
	dma_fill((uint8_t *)mapdata_offset + offset, tile_id, length);
}

// x = 0 - 31, y = 0 - 23
// This needs updating to support bank switching / 512 tile id
void tilemap_set_tile_id(uint8_t x, uint8_t y, uint8_t tile_id, uint16_t mapdata_offset)
{
	((uint8_t *)mapdata_offset)[x + y * 40] = tile_id;
}

uint8_t tilemap_get_tile_id(uint8_t x, uint8_t y, uint16_t mapdata_offset)
{
	return ((uint8_t *)mapdata_offset)[x + y * 40];
}

void *tilemap_get_tile_addr(uint8_t x, uint8_t y, uint16_t mapdata_offset)
{
	return &((uint8_t *)mapdata_offset)[x + y * 40];
}
