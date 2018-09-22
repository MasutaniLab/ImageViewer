@echo off
pushd %~dp0
set Path=..\ZBarWin64\lib;..\ZBarWin64\zbar\libiconv\dll_x64;%Path%
if not defined RTM_VC_CONFIG set RTM_VC_CONFIG=Release
start "%~n0 %1 %2 %3 %4 %5 %6 %7 %8 %9" "build\src\%RTM_VC_CONFIG%\%~n0Comp.exe" %1 %2 %3 %4 %5 %6 %7 %8 %9
popd
