@ECHO OFF
:: CLEANUP

:: Walk through build-targets.txt and build each target
FOR /F "eol=# tokens=1,2" %%I in (build-targets.txt) DO (
  FOR /F "delims=v. tokens=1,2" %%K IN ("%%J") DO (
    ECHO == Building for %%I architecture and node v%%K.%%L.*
    CALL node-gyp install rebuild --arch=%%I --target=%%J
    IF ERRORLEVEL 1 (
      ECHO Build for %%I architecture and node v%%K.%%L.* failed.
      EXIT /B %ERRORLEVEL%
    )
    :: Copy binary to sub-folder
    IF NOT EXIST "bin\%%I\%%K.%%L" MKDIR "bin\%%I\%%K.%%L"
    COPY "build\Release\*.node" "bin\%%I\%%K.%%L\" 2>&1 >nul
    IF ERRORLEVEL 1 (
      ECHO Cannot copy build for %%I architecture and node v%%K.%%L.* to "bin\%%I\%%K.%%L\" folder.
      EXIT /B %ERRORLEVEL%
    )
    :: Delete obsolete build folder
    CALL node-gyp clean
  )
)
EXIT /B 0
