@ECHO OFF
IF NOT DEFINED CSPECT_HOME SET CSPECT_HOME=..\..\..\..\NextTools\CSpect
CD %~dp0\..\bin
%CSPECT_HOME%\CSpect.exe -brk -w4 -s28 -r -tv -zxnext -mmc=.\ zxnext_tilemap.nex
