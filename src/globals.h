#ifndef _GAME_H
#define _GAME_H

#define RTM_28MHZ				0x03

#define WFRAMES					4

#define MMU_LAYER2				MMU_C000
#define PAGE_LAYER2				8

#define MMU_TILE_MAP			MMU_0000
#define PAGE_TILE_MAP			17

#define MMU_TILE_DATA			MMU_0000
#define PAGE_TILE_DATA			18

#define MMU_TILES				MMU_4000
#define PAGE_TILES				5

#define TILE_X					8
#define TILE_Y					8

#define RES_256x192				0
#define RES_320x256				1
#define RES_640x256				2

#define SCREEN_RES				RES_320x256

#if (SCREEN_RES == RES_256x192)
#define SCREEN_X				256
#define SCREEN_Y				192
#elif (SCREEN_RES == RES_320x256)
#define SCREEN_X				320
#define SCREEN_Y				256
#elif (SCREEN_RES == RES_640x256)
#define SCREEN_X				640
#define SCREEN_Y				256
#endif

#define SCREEN_SIZE				(SCREEN_X * SCREEN_Y)
#define SCREEN_TILES_X			(SCREEN_X / TILE_X)
#define SCREEN_TILES_Y			(SCREEN_Y / TILE_Y)
#define SCREEN_TILES_COUNT		(SCREEN_TILES_X * SCREEN_TILES_Y)

#define MIN(a, b)				(((a) < (b)) ? (a) : (b))
#define MAX(a, b)				(((a) > (b)) ? (a) : (b))
#define CLAMP(x, lower, upper)	(MIN(upper, MAX(x, lower)))
#define printAt(col, row, str)	printf("\x16%c%c%s", (col), (row), (str))

extern void breakpoint();

#endif
