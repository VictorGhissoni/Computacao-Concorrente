@echo off
setlocal enabledelayedexpansion

echo Comparing all test results with gabarito...
echo.

set total_tests=0
set passed_tests=0
set failed_tests=0

REM Process each test file
for %%f in (logs\test_*.log) do (
    set /a total_tests+=1
    set "test_file=%%f"
    set "filename=%%~nxf"
    
    echo.
    echo ============================================
    echo Processing: !filename!
    echo ============================================
    
    REM Extract argument from test filename
    REM Format: test_ARGUMENT_*_*_timestamp.log
    REM The argument is the first number after "test_"
    set "basename=%%~nf"
    
    REM Extract the first token after "test_"
    for /f "tokens=2 delims=_" %%a in ("!basename!") do (
        set "argument=%%a"
    )
    
    echo Extracted argument: !argument!
    
    REM Find matching gabarito file with EXACT argument match
    set "gabarito_file="
    
    REM Try exact match first: gabarito_ARGUMENT.log
    if exist "logs\gabarito_!argument!.log" (
        set "gabarito_file=logs\gabarito_!argument!.log"
    )
    
    REM Try with timestamp: gabarito_ARGUMENT_*.log
    if not defined gabarito_file (
        for %%g in (logs\gabarito_!argument!_*.log) do (
            set "gabarito_file=%%g"
        )
    )
    
    if not defined gabarito_file (
        echo ERROR: No matching gabarito found for argument !argument!
        set /a failed_tests+=1
        goto :next_test
    )
    
    echo Test file: !test_file!
    echo Gabarito file: !gabarito_file!
    echo.
    
    REM Extract the "primos totais:" line from both files
    findstr /B /C:"primos totais:" "!test_file!" > temp_test.txt 2>nul
    findstr /B /C:"primos totais:" "!gabarito_file!" > temp_gabarito.txt 2>nul
    
    REM Check if lines were found
    if not exist temp_test.txt (
        echo ERROR: "primos totais:" not found in test file
        set /a failed_tests+=1
        goto :next_test
    )
    
    if not exist temp_gabarito.txt (
        echo ERROR: "primos totais:" not found in gabarito file
        set /a failed_tests+=1
        goto :next_test
    )
    
    REM Show what was extracted
    echo Test output:
    type temp_test.txt
    echo Gabarito output:
    type temp_gabarito.txt
    echo.
    
    REM Compare the extracted lines
    fc temp_test.txt temp_gabarito.txt >nul 2>&1
    
    if !errorlevel! equ 0 (
        echo Result: *** PASSED *** - Outputs match!
        set /a passed_tests+=1
    ) else (
        echo Result: *** FAILED *** - Outputs differ!
        set /a failed_tests+=1
    )
    
    :next_test
    del temp_test.txt temp_gabarito.txt 2>nul
)

echo.
echo.
echo ============================================
echo SUMMARY
echo ============================================
echo Total tests: %total_tests%
echo Passed: %passed_tests%
echo Failed: %failed_tests%
echo ============================================
echo.

if %failed_tests% gtr 0 (
    echo Some tests FAILED!
) else if %passed_tests% gtr 0 (
    echo All tests PASSED!
) else (
    echo No tests were run!
)

echo.
echo Press any key to exit...
pause >nul