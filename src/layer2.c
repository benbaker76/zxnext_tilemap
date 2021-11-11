#include "layer2.h"
#include <arch/zxn.h>
#include <intrinsic.h>
#include <z80.h>
#include <string.h>
#include "bank.h"
#include "globals.h"
#include "dma.h"

void layer2_set_screen(char *p_image, uint8_t image_bank, uint8_t image_page)
{
	void *screen_address = (void *) zxn_addr_from_mmu(MMU_LAYER2);
	uint8_t screen_page = ZXN_READ_REG(REG_LAYER_2_RAM_PAGE);
	uint8_t old_page = bank_set_16k(image_bank, image_page);
	
#if (SCREEN_X == RES_256x192)
	for (uint8_t i = 0; i < 3; i++)
#else
	for (uint8_t i = 0; i < 5; i++)
#endif
	{
		bank_set_16k(image_bank, image_page + i);
		bank_set_16k(MMU_LAYER2, screen_page + i);
		memcpy(screen_address, (void *)p_image[i], BANKSIZE_16K);
	}

	bank_set_16k(image_bank, old_page);
}

void layer2_clear_screen(uint8_t value)
{
	void *screen_address = (void *)zxn_addr_from_mmu(MMU_LAYER2);
#if (SCREEN_RES == RES_256x192)
	for (uint8_t i = 0; i < 3; i++)
#else
	for (uint8_t i = 0; i < 5; i++)
#endif
	{
		bank_set_16k(MMU_LAYER2, PAGE_LAYER2 + i);
		dma_fill(screen_address, value, BANKSIZE_16K);
	}
}