@ECHO OFF
IF "%~1" == "install" (
  ECHO This module comes with pre-compiled binaries.
  ECHO To rebuild them all run "build.cmd". You can also provide a processor
  ECHO architecture and node version to build a single .node file, eg.
  ECHO    build.cmd ia32 0.12.0
) ELSE IF "%~1" == "" (
  :: Walk through build-targets.txt and build each target
  FOR /F "eol=# tokens=1,2" %%I in (build-targets.txt) DO (
    CALL build.cmd %%I %%J
    IF ERRORLEVEL 1 EXIT /B %ERRORLEVEL%
  )
) ELSE (
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
)
EXIT /B 0
