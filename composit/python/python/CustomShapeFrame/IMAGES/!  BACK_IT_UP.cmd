@ECHO OFF
SETLOCAL
PUSHD "%CD%"
CD /D "%~DP0"
SET PROMPT=$G 

:: --------------------------------------------


@:: Delete any *file* that happens to be named "backup".
IF EXIST BACKUP.   DEL /F /Q BACKUP.

@:: Create the folder if necessary.
IF NOT EXIST BACKUP\    MD BACKUP

@:: Copy the files and display a more intelligent list of files that are copied.
ECHO.
COPY /Y /V *.* BACKUP\  | FIND.exe /I /V "copied"  | FIND.exe /I /V "%~N0"


:: --------------------------------------------

:AllDone
    POPD
    ENDLOCAL
    GOTO :EOF

