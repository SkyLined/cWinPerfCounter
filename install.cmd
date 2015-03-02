@ECHO OFF
:: Walk through build-targets.txt and build each target
SET NODE_MAYOR_VERSION=
FOR /F "usebackq tokens=1,2,3 delims=v." %%I IN (`node -v`) DO (
  SET NODE_MAYOR_VERSION=%%I.%%J
  SET NODE_MINOR_VERSION=%%K
)
IF "%NODE_MAYOR_VERSION%" == "" (
  ECHO Cannot determine node version.
  EXIT /B 1
)
SET NODE_ARCH=
FOR /F "usebackq tokens=1 delims=" %%I in (`node -e console.log^(process.arch^)`) DO (
  SET NODE_ARCH=%%I
)
IF "%NODE_ARCH%" == "ia32" (
  REM OK
) ELSE IF "%NODE_ARCH%" == "x64" (
  REM OK
) ELSE (
  ECHO Cannot determine node processor architecture.
  EXIT /B 1
)
IF EXIST "addon\%NODE_ARCH%\%NODE_MAYOR_VERSION%" (
  RMDIR "addon\%NODE_ARCH%\%NODE_MAYOR_VERSION%" /S /Q
)
ECHO == Building for %NODE_ARCH% architecture and node v%NODE_MAYOR_VERSION%.%NODE_MINOR_VERSION%
CALL node-gyp install rebuild --arch=%NODE_ARCH% --target=%NODE_MAYOR_VERSION%.%NODE_MINOR_VERSION%
IF ERRORLEVEL 1 (
  ECHO Build for %NODE_ARCH% architecture and node v%NODE_MAYOR_VERSION%.%NODE_MINOR_VERSION% failed.
  EXIT /B %ERRORLEVEL%
)
:: Copy binary to sub-folder of "addon" folder
IF NOT EXIST "addon\%NODE_ARCH%\%NODE_MAYOR_VERSION%" MKDIR "addon\%NODE_ARCH%\%NODE_MAYOR_VERSION%"
COPY "build\Release\*.node" "addon\%NODE_ARCH%\%NODE_MAYOR_VERSION%\" 2>&1 >nul
IF ERRORLEVEL 1 (
  ECHO Cannot copy build for %NODE_ARCH% architecture and node v%NODE_MAYOR_VERSION%.%NODE_MINOR_VERSION% to "addon\%NODE_ARCH%\%NODE_MAYOR_VERSION%\" folder.
  EXIT /B %ERRORLEVEL%
)
:: Delete obsolete build folder
CALL node-gyp clean

EXIT /B 0
