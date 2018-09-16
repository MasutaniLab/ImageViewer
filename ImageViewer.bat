@echo off
pushd %~dp0
set Path=..\ZBarWin64\lib;..\ZBarWin64\zbar\libiconv\dll_x64;%Path%
if not defined RTM_VC_CONFIG set RTM_VC_CONFIG=Release
start "%~n0" "build\src\%RTM_VC_CONFIG%\%~n0Comp.exe"
popd
