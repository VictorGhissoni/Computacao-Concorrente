@echo off
setlocal enabledelayedexpansion

REM Create logs directory
if not exist logs mkdir logs

REM Define sets for cartesian product
set "set_a=100 1000 10000 100000"
set "set_b=5 10 15 25 32"
set "set_c=4 8"

echo Generating cartesian product of test cases...
echo Set A: %set_a%
echo Set B: %set_b%
echo Set C: %set_c%
echo.

set test_num=1

REM Nested loops for cartesian product
for %%a in (%set_a%) do (
    for %%b in (%set_b%) do (
        for %%c in (%set_c%) do (
            REM Create timestamp
            set timestamp=%date:~-4%%date:~3,2%%date:~0,2%_%time:~0,2%%time:~3,2%%time:~6,2%
            set timestamp=!timestamp: =0!
            
            REM Create log filename
            set logfile=logs\test_%%a_%%b_%%c_!timestamp!.log
            
            echo Test !test_num!: Running .\exercicio_3.exe %%a %%b %%c
            echo Logging to: !logfile!
            
            REM Run the program and log output
            (
                echo === Test Run ===
                echo Arguments: %%a %%b %%c
                echo Timestamp: %date% %time%
                echo === Output ===
                .\exercicio_3.exe %%a %%b %%c
                echo.
                echo === Exit Code: !errorlevel! ===
            ) > "!logfile!" 2>&1
            
            echo Test !test_num! complete
            echo ---
            set /a test_num+=1
        )
    )
)

echo.
echo All tests complete. Logs saved in .\logs\
pause