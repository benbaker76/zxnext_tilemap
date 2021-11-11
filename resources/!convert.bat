@ECHO OFF
IF NOT DEFINED GFX2NEXT_HOME SET GFX2NEXT_HOME=..\..\..\..\NextTools\Gfx2Next\bin
%GFX2NEXT_HOME%\gfx2next.exe -tile-norotate -map-16bit -colors-4bit -z80asm -bank-sections=rodata_user,BANK_17,BANK_18 tiles.png
move *.nxt ..\binary >nul
move *.nxm ..\binary >nul
move *.nxp ..\binary >nul
move *.asm ..\src >nul
move *.h ..\src >nul
