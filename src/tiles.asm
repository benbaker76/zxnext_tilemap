
SECTION rodata_user

PUBLIC _tiles_nxp
PUBLIC _tiles_nxp_end

_tiles_nxp:

	BINARY "binary/tiles.nxp"	; 32 bytes

_tiles_nxp_end:

SECTION BANK_17

PUBLIC _tiles_nxm
PUBLIC _tiles_nxm_end

_tiles_nxm:

	BINARY "binary/tiles.nxm"	; 4608 bytes

_tiles_nxm_end:

SECTION BANK_18

PUBLIC _tiles_nxt
PUBLIC _tiles_nxt_end

_tiles_nxt:

	BINARY "binary/tiles.nxt"	; 4800 bytes

_tiles_nxt_end:
