#!/bin/bash
set -x

#echo executing make_jni_header.sh N_Cef ...
./make_jni_header.sh N_Cef

#echo executing make_jni_header.sh N_CefBrowser ...
./make_jni_header.sh N_CefBrowser

