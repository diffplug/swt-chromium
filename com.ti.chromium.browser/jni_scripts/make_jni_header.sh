#!/bin/bash
set -x

JDK_BIN="/home/patrick/CCS_Dev/jdk1.6.0_13/bin"
CLS_PATH="../bin"
OUT_PATH="../src_native/CEFBridge/src"

#echo JDK_BIN=$JDK_BIN
#echo CLS_PATH==$CLS_PATH
#echo OUT_PATH=$OUT_PATH

#echo executing $JDK_BIN/javah -force -classpath $CLS_PATH -d $OUT_PATH  com.ti.chromium.browser.cef.$1

$JDK_BIN/javah -force -classpath $CLS_PATH -d $OUT_PATH  com.ti.chromium.browser.cef.$1

rm -f $OUT_PATH/$1.h
mv $OUT_PATH/com_ti_chromium_browser_cef_$1.h $OUT_PATH/$1.h


