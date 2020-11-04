@echo off
SET VC_DIR=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build


SET QT5_10=C:\Qt\Qt5.10.0\5.10.0\msvc2017_64\bin
SET QT5_14=C:\Qt\Qt5.14.2\5.14.2\msvc2017_64\bin


SET "PATH=%QT5_14%;%PATH%"
call "%VC_DIR%\vcvars64.bat"
qmake  -r -tp vc  SunnyTcsApi.pro

pause