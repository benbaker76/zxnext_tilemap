#include <arch/zxn.h>
#include <z80.h>
#include <intrinsic.h>
#include "dma.h"

typedef struct
{
	uint8_t disable;
	uint8_t wr0;
	void *source;
	uint16_t length;
	uint8_t wr1;
	uint8_t timing_a;
	uint8_t wr2;
	uint8_t timing_b;
	uint8_t wr4;
	void *dest;
	uint8_t wr5;
	uint8_t load;
	uint8_t enable;
} dma_code_t;

typedef struct
{
	uint8_t fill_value;
	uint8_t disable;
	uint8_t wr0;
	void *source;
	uint16_t length;
	uint8_t wr1;
	uint8_t wr2;
	uint8_t wr4;
	void *dest;
	uint8_t load;
	uint8_t enable;
} dma_code_fill_t;

typedef struct
{
	uint8_t disable_dma;
	uint8_t reset_dma;
	uint8_t reset_port_a;
	uint8_t timing_a_b;
	uint8_t wr0;
	void *source;
	uint16_t length;
	uint8_t wr1;
	uint8_t timing_a;
	uint8_t wr2;
	uint8_t wr2_scaler;
	uint8_t scaler;
	uint8_t wr4;
	void *dest;
	uint8_t wr5;
	uint8_t read_mask;
	uint8_t mask;
	uint8_t load;
	uint8_t force;
	uint8_t enable;
} dma_code_sample_t;

dma_code_t dma_code_transfer =
{
	.disable = D_DISABLE_DMA,
	.wr0 = D_WR0 | D_WR0_X56_LEN | D_WR0_X34_A_START | D_WR0_TRANSFER_A_TO_B,
	.source = 0,
	.length = 0,
	.wr1 = D_WR1_X6_A_TIMING | D_WR1_A_IS_MEM_INC,
	.timing_a = D_WR1X6_A_CLEN_2,
	.wr2 = D_WR2_X6_B_TIMING | D_WR2_B_IS_MEM_INC,
	.timing_b = D_WR2X6_B_CLEN_2,
	.wr4 = 0x80 | D_WR4_X23_B_START | D_WR4_CONT | 0x01,
	.dest = 0,
	.wr5 = D_WR5,
	.load = D_LOAD,
	.enable = D_ENABLE_DMA
};

dma_code_t dma_code_transfer_reversed =
{
	.disable = D_DISABLE_DMA,
	.wr0 = D_WR0 | D_WR0_X56_LEN | D_WR0_X34_A_START | D_WR0_TRANSFER_A_TO_B,
	.source = 0,
	.length = 0,
	.wr1 = D_WR1_X6_A_TIMING | D_WR1_A_IS_MEM_DEC,
	.timing_a = D_WR1X6_A_CLEN_2,
	.wr2 = D_WR2_X6_B_TIMING | D_WR2_B_IS_MEM_DEC,
	.timing_b = D_WR2X6_B_CLEN_2,
	.wr4 = 0x80 | D_WR4_X23_B_START | D_WR4_CONT | 0x01,
	.dest = 0,
	.wr5 = D_WR5,
	.load = D_LOAD,
	.enable = D_ENABLE_DMA
};

dma_code_t dma_code_transfer_io =
{
	.disable = D_DISABLE_DMA,
	.wr0 = D_WR0 | D_WR0_X56_LEN | D_WR0_X34_A_START | D_WR0_TRANSFER_A_TO_B,
	.source = 0,
	.length = 0,
	.wr1 = D_WR1_X6_A_TIMING | D_WR1_A_IS_MEM_INC,
	.timing_a = D_WR1X6_A_CLEN_2,
	.wr2 = D_WR2_X6_B_TIMING | D_WR2_B_IS_IO_FIX,
	.timing_b = D_WR2X6_B_CLEN_2,
	.wr4 = 0x80 | D_WR4_X23_B_START | D_WR4_CONT | 0x01,
	.dest = 0,
	.wr5 = D_WR5,
	.load = D_LOAD,
	.enable = D_ENABLE_DMA
};

dma_code_fill_t dma_code_fill =
{
	.fill_value = 0,
	.disable = D_DISABLE_DMA,
	.wr0 = D_WR0 | D_WR0_X56_LEN | D_WR0_X34_A_START | D_WR0_TRANSFER_A_TO_B,
	.source = 0,
	.length = 0,
	.wr1 = D_WR1_A_IS_MEM_FIX,
	.wr2 = D_WR2_B_INC,
	.wr4 = 0x80 | D_WR4_X23_B_START | D_WR4_CONT | 0x01,
	.dest = 0,
	.load = D_LOAD,
	.enable = D_ENABLE_DMA
};

dma_code_sample_t dma_code_sample_io =
{
	.disable_dma = D_DISABLE_DMA,
	.reset_dma = 0xc3,
	.reset_port_a = 0xc7,
	.timing_a_b = 0xcb,
	.wr0 = D_WR0 | D_WR0_X56_LEN | D_WR0_X34_A_START | D_WR0_TRANSFER_A_TO_B,
	.source = 0,
	.length = 8192,
	.wr1 = D_WR1_X6_A_TIMING | D_WR1_A_IS_MEM_INC,
	.timing_a = D_WR1X6_A_CLEN_2,
	.wr2 = D_WR2_X6_B_TIMING | D_WR2_B_IS_IO_FIX,
	.wr2_scaler = D_WR2X6_X5_PRESCALAR | D_WR2X6_B_CLEN_2,
	.scaler = 8*SAMPLE_SCALER,
	.wr4 = 0x80 | D_WR4_X23_B_START | D_WR4_BURST | 0x01,
	.dest = SAMPLE_COVOXPORT,
	.wr5 = SAMPLE_LOOP,
	.read_mask = D_READ_MASK,
	.mask = 0b00001000,
	.load = D_LOAD,
	.force = 0xb3,
	.enable = D_ENABLE_DMA
};

void dma_transfer(void *dest, void *source, uint16_t length)
{
	dma_code_transfer.source = source;
	dma_code_transfer.length = length;
	dma_code_transfer.dest = dest;
	
	z80_otir(&dma_code_transfer, (uint8_t)&IO_DMA, sizeof(dma_code_transfer));
}

void dma_transfer_reverse(void *dest, void *source, uint16_t length)
{
	dma_code_transfer_reversed.source = source;
	dma_code_transfer_reversed.length = length;
	dma_code_transfer_reversed.dest = dest;
	
	z80_otir(&dma_code_transfer_reversed, (uint8_t)&IO_DMA, sizeof(dma_code_transfer_reversed));
}

void dma_transfer_port(void *source, uint16_t length)
{
	dma_code_transfer_io.source = source;
	dma_code_transfer_io.length = length;
	dma_code_transfer_io.dest = (void *)(uint8_t)&IO_NEXTREG_DAT;
	
	z80_otir(&dma_code_transfer_io, (uint8_t)&IO_DMA, sizeof(dma_code_transfer_io));
}

void dma_transfer_sprite(void *source, uint16_t length)
{
	dma_code_transfer_io.source = source;
	dma_code_transfer_io.length = length;
	dma_code_transfer_io.dest = (void *)(uint8_t)&IO_SPRITE_PATTERN;
	
	z80_otir(&dma_code_transfer_io, (uint8_t)&IO_DMA, sizeof(dma_code_transfer_io));
}

void dma_transfer_sample(void *source, uint16_t length, uint8_t scaler, bool loop)
{
	dma_code_sample_io.source = source;
	dma_code_sample_io.length = length;
	dma_code_sample_io.scaler = scaler;
	dma_code_sample_io.wr5 = (loop ? SAMPLE_LOOP : SAMPLE_NOLOOP);
	dma_code_sample_io.dest = (void *)SAMPLE_COVOXPORT;

	z80_otir(&dma_code_sample_io, (uint8_t)&IO_DMA, sizeof(dma_code_sample_io));
}

void dma_fill(void *dest, uint8_t fill_value, uint16_t length)
{
	dma_code_fill.fill_value = fill_value;
	dma_code_fill.source = &dma_code_fill.fill_value;
	dma_code_fill.length = length;
	dma_code_fill.dest = dest;
	
	z80_otir(&dma_code_fill, (uint8_t)&IO_DMA, sizeof(dma_code_fill));
}
