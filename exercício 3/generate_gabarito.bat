@echo off
setlocal enabledelayedexpansion

REM Create logs directory
if not exist logs mkdir logs

REM Number of test cases to generate
set num_tests=4


:: calculate x^n
SET x=10
SET result=10

echo Generating and running %num_tests% test cases...
echo.

FOR /L %%i IN (1,1,%num_tests%) DO (
    SET /A result*=x
    REM Create log filename
    set logfile=logs\gabarito_!result!.log
    
    echo Test %%i: Running .\primos_concorrente.exe !result!
    echo Logging to: !logfile!
    
    REM Run the program and log output
    (
        echo === Test Run ===
        echo Arguments: !result!
        echo === Output ===
        .\primos_concorrente.exe !result!
        echo.
        echo === Exit Code: !errorlevel! ===
    ) > "!logfile!" 2>&1
    
    echo Test %%i complete
    echo ---    
)
    
ECHO %result%
pause
