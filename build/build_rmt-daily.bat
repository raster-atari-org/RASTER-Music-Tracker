@echo off
cd "%~dp0"

set RELEASE=Rmt
set BASE_DIR=C:\jac\system\Windows\Programming\Repositories\RASTER-Music-Tracker
set TARGET_FILE=rmt135-daily.zip

set PRODUCTIONS=C:\jac\system\WWW\Sites\www.wudsn.com\productions
set TARGET_DIR=%PRODUCTIONS%\windows\rastermusictracker
set WINRAR=C:\jac\system\Windows\Tools\FIL\WinRAR\winrar
set UPLOAD=%PRODUCTIONS%\www\site\export\upload.bat

set MSBUILD="C:\Program Files\Microsoft Visual Studio\18\Community\Msbuild\Current\Bin\MSBuild.exe"
if not exist %MSBUILD% goto :msbuild_missing_error

REM When using the correct MSBUILD, no separate setting of target path seems to be required
REM set VCTargetsPath="C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Microsoft\VC\v170"
set SLN=%BASE_DIR%\%RELEASE%.sln
set RELEASE_BASE_DIR=%TEMP%\%RELEASE%\
rmdir /S /Q %RELEASE_BASE_DIR%
mkdir %RELEASE_BASE_DIR%

set CONFIGURATION=Debug
call :build_configuration
set CONFIGURATION=Release
call :build_configuration

call :upload
echo Done.
pause
goto :eof

:build_configuration
set CONFIGURATION_DIR=%CONFIGURATION%
set OUTPUT_DIR=%BASE_DIR%\out\%CONFIGURATION_DIR%\output
set RESULT_EXE=%OUTPUT_DIR%\%RELEASE%.exe
echo INFO: Buidling %RESULT_EXE% for configuration %CONFIGURATION%.
if exist %RESULT_EXE% del %RESULT_EXE%
%MSBUILD% %SLN% /property:Configuration=%CONFIGURATION% -fl -flp:logfile=%OUTPUT_DIR%\msbuild.log
if not exist %RESULT_EXE% goto :build_failed_error

call :copy_output
goto :eof

:upload
echo on
set TARGET=%TARGET_DIR%\%TARGET_FILE%
del %TARGET%
cd %RELEASE_BASE_DIR%\..

%WINRAR% a -afzip -x*.log %TARGET% %RELEASE%
if ERRORLEVEL 1 goto :error
start %TARGET_DIR%
cd %TARGET_DIR%


call %UPLOAD% productions
goto :eof

:copy_output
set RELEASE_DIR=%RELEASE_BASE_DIR%\%CONFIGURATION%
mkdir %RELEASE_DIR%
xcopy /E /Y /EXCLUDE:build_rmt-daily-excluded-extensions.txt %OUTPUT_DIR%  %RELEASE_DIR%
rem Exclude the .ini files from the download to prevent users from accidentally overwriting them.
if exist %RELEASE_DIR%\%RELEASE%.ini del %RELEASE_DIR%\%RELEASE%.ini
if exist %RELEASE_DIR%\tuning.ini del %RELEASE_DIR%\tuning.ini
start %RELEASE_DIR%
goto :eof

:msbuild_missing_error
echo ERROR: %MSBUILD% not present.
goto :error

:build_failed_error
echo ERROR: %RESULT_EXE% was not created.

:error
echo ERROR: See error messages above.
pause
exit
