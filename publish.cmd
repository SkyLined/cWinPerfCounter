@ECHO OFF
CALL npm version patch -m "Published version %%%%s"
CALL npm publish