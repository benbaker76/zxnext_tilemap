#ifndef _BANK_H
#define _BANK_H

#include <stdint.h>
#include <stdbool.h>

#define MMU_0000				0
#define MMU_2000				1
#define MMU_4000				2
#define MMU_6000				3
#define MMU_8000				4
#define MMU_A000				5
#define MMU_C000				6
#define MMU_E000				7

#define BANKSIZE_16K			16384
#define BANKSIZE_8K				8192

uint8_t bank_set_8k(uint8_t mmu, uint8_t page);
uint8_t bank_set_16k(uint8_t mmu, uint8_t page);
void bank_set_esx();

#endif
