@ECHO OFF
CALL npm version patch -m "Updated to version %%%%s"
CALL npm publish