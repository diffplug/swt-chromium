@echo off
setlocal

SET JDK_BIN="C:\Program Files (x86)\Java\jdk1.6.0_37\bin"
SET CLS_PATH="..\bin"
SET OUT_PATH="..\src_native\CEFBridge\src"

CALL %JDK_BIN%\javah.exe -force -classpath %CLS_PATH% -d %OUT_PATH% com.ti.chromium.browser.cef.%1
del /F %OUT_PATH%\%1.h
rename %OUT_PATH%\com_ti_chromium_browser_cef_%1.h %1.h
