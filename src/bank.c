#include <arch/zxn.h>
#include <z80.h>

uint8_t bank_set_8k(uint8_t mmu, uint8_t page)
{
	uint8_t old_page = ZXN_READ_REG(REG_MMU0 + mmu);
	page <<= 1;
	ZXN_WRITE_REG(REG_MMU0 + mmu, page);
	return old_page >> 1;
}

uint8_t bank_set_16k(uint8_t mmu, uint8_t page)
{
	uint8_t old_page = ZXN_READ_REG(REG_MMU0 + mmu);
	page <<= 1;
	ZXN_WRITE_REG(REG_MMU0 + mmu, page);
	ZXN_WRITE_REG(REG_MMU0 + mmu + 1, page + 1);
	return old_page >> 1;
}

void bank_set_esx()
{
	ZXN_WRITE_MMU0(255);
	ZXN_WRITE_MMU1(255);
}
