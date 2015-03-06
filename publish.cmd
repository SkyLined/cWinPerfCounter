@ECHO OFF
:START
  ECHO === Updating version number...
  CALL npm version patch -m "Version updated to %%%%s"
  IF ERRORLEVEL 1 (
    ECHO === Cannot update version number!
    GOTO :ERROR
  )
  CALL npm publish
  IF ERRORLEVEL 1 (
    ECHO === Cannot publish package!
    ECHO === Reverting version update commit...
    git reset --hard HEAD~1
    IF ERRORLEVEL 1 (
      ECHO === Cannot revert commit to update version number!
    )
    GOTO :ERROR
  )
  EXIT /B 0

:ERROR
  ECHO Press CTRL+C to terminate or ENTER to retry...
  PAUSE >nul
  GOTO :START

