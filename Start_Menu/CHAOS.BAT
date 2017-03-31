echo off
rem            **** James Gleick's CHAOS:The Software ****
rem                       Main menu batch file
rem
rem                       EDIT AT YOUR OWN RISK
metashel /i
set batchval=1
:top
menu %batchval%
if errorlevel 999 goto exit
if errorlevel 6 goto toy
if errorlevel 5 goto forge
if errorlevel 4 goto barn
if errorlevel 3 goto attract
if errorlevel 2 goto mag
if errorlevel 1 goto mand
goto top

:mand
set batchval=2
mandelx %1
if errorlevel 255 goto top
if errorlevel 2 goto exit
set batchval=0
goto top

:mag
set batchval=3
magnetx %1
if errorlevel 255 goto top
if errorlevel 2 goto exit
set batchval=0
goto top

:attract
set batchval=4
attractx %1
if errorlevel 255 goto top
if errorlevel 2 goto exit
set batchval=0
goto top

:barn
set batchval=5
gamex %1
if errorlevel 255 goto top
if errorlevel 2 goto exit
set batchval=0
goto top

:forge
set batchval=6
forgex %1
if errorlevel 255 goto top
if errorlevel 2 goto exit
set batchval=0
goto top

:toy
set batchval=7
toyx %1
if errorlevel 255 goto top
if errorlevel 2 goto exit
set batchval=0
goto top


:exit
set batchval=
metashel /k
cls
echo Thank you for using CHAOS: The Software


