SECTION rodata_user

PUBLIC _breakpoint
	
_breakpoint:
	DEFB $dd, $01
	ret
