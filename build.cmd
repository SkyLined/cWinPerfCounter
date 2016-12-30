@ECHO OFF
IF "%~1" == "install" (
  ECHO This module comes with pre-compiled binaries.
  ECHO To rebuild them all run "build.cmd". You can also provide a processor
  ECHO architecture and node version to build a single .node file, eg.
  ECHO    build.cmd ia32 0.12.0
  EXIT /B 0
)
WHERE npm 2>&1 >nul
IF ERRORLEVEL 1 (
  ECHO It appears you do not have npm installed yet.
  ECHO Please install npm before attempting to build this package.
  ECHO Go to http://npmjs.org for details.
)
CALL npm update
IF ERRORLEVEL 1 EXIT /B %ERRORLEVEL%
WHERE node-gyp 2>&1 >nul
IF ERRORLEVEL 1 (
  ECHO It appears you do not have node-gyp installed yet.
  ECHO Please install node-gyp before attempting to build this package.
  ECHO     npm install -g node-gyp
  ECHO node-gyp has some additional dependencies, such as Python.
  ECHO Go to https://github.com/nodejs/node-gyp for details.
  EXIT /B 1
)
IF "%~1" == "" (
  :: Walk through build-targets.txt and build each target
  FOR /F "eol=# tokens=1,2" %%I in (build-targets.txt) DO (
    CALL :BUILD_TARGET %%I %%J
    IF ERRORLEVEL 1 EXIT /B %ERRORLEVEL%
  )
  EXIT /B 0
)

CALL :BUILD_TARGET %1 %2
EXIT /B %ERRORLEVEL%

:BUILD_TARGET
  TITLE Building for %1 architecture and node %2
  CALL node-gyp install rebuild --arch=%1 --target=%2
  IF ERRORLEVEL 1 (
    ECHO Build for %1 architecture and node %2 failed.
    EXIT /B %ERRORLEVEL%
  )
  :: Copy binary to sub-folder
  IF NOT EXIST "bin\%1\%2" MKDIR "bin\%1\%2"
  COPY "build\Release\*.node" "bin\%1\%2\" 2>&1 >nul
  IF ERRORLEVEL 1 (
    ECHO Cannot copy build to "bin\%1\%2\" folder.
    EXIT /B %ERRORLEVEL%
  )
  :: Delete obsolete build folder
  CALL node-gyp clean
  TITLE Building finished
  EXIT /B 0
