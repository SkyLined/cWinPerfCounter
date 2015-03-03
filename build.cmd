@ECHO OFF
IF "%~1" == "install" (
  ECHO This module comes with pre-compiled binaries. To rebuild them run "build.cmd".
) ELSE (
  :: Walk through build-targets.txt and build each target
  FOR /F "eol=# tokens=1,2" %%I in (build-targets.txt) DO (
    ECHO == Building for %%I architecture and node %%J
    CALL node-gyp install rebuild --arch=%%I --target=%%J
    IF ERRORLEVEL 1 (
      ECHO Build for %%I architecture and node v%%J failed.
      EXIT /B %ERRORLEVEL%
    )
    :: Copy binary to sub-folder
    IF NOT EXIST "bin\%%I\%%J" MKDIR "bin\%%I\%%J"
    COPY "build\Release\*.node" "bin\%%I\%%J\" 2>&1 >nul
    IF ERRORLEVEL 1 (
      ECHO Cannot copy build to "bin\%%I\%%J\" folder.
      EXIT /B %ERRORLEVEL%
    )
    :: Delete obsolete build folder
    CALL node-gyp clean
  )
)
EXIT /B 0
