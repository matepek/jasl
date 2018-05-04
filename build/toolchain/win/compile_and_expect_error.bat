@echo off

set output=%1
shift

set RESTVAR=
:loop1
if ""=="%1" goto after_loop
set RESTVAR=%RESTVAR% %1
shift
goto loop1
:after_loop

call %RESTVAR% > NUL

IF %ERRORLEVEL% NEQ 0 (
  type nul > %output%
  exit 0
)

del %output%
echo Error: Compilation error was expected, but not occured.
exit 1