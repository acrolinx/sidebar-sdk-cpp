@echo off
:: RegisterAcrolinx
::-------------------------------------
REM  --> Check for permissions
>nul 2>&1 "%SYSTEMROOT%\system32\icacls.exe" "%SYSTEMROOT%\system32\config\system"

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params = %*:"="
	echo "%~s0 %params%"
    echo UAC.ShellExecute "cmd.exe", "/c %~s0 %params%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
	exit /B
:gotAdmin
    pushd "%CD%"
    CD /D "%~dp0"
::--------------------------------------
REM  --> Check if dll exist then register
if exist "Acrolinx.Sidebar.SDK.dll" (
    regsvr32  Acrolinx.Sidebar.SDK.dll
) else (
    echo DLL not found
	pause
)
exit /B

