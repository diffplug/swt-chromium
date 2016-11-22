#ifndef _SYSTEM_UTILTITIES_H_
#define _SYSTEM_UTILTITIES_H_
#pragma once

#include <assert.h>
#include <jni.h>
#include <include/cef_app.h>

#define REQUIRE_UI_THREAD()   assert(CefCurrentlyOn(TID_UI));
#define REQUIRE_IO_THREAD()   assert(CefCurrentlyOn(TID_IO));
#define REQUIRE_FILE_THREAD() assert(CefCurrentlyOn(TID_FILE));

/**
 * Returns the JNI environment object for the current thread.
 */
JNIEnv* GetJNIEnv();

/**
 * Retrieve a String value.
 */
CefString GetStringFromJNI(JNIEnv* env, jstring jstr);

/**
 * Create a new String value.
 */
jstring CreateJNIString(JNIEnv* env, const CefString& str);

/** 
 * Set the CEF object for an existing JNI object.  A reference will be
 * added to the object.  If a previous object existed a reference
 * will be removed from that object.
*/
bool SetCefForJNIObject(JNIEnv* env, jobject obj, CefBrowserHost* base);

/**
 * Returns the CEF object from an existing JNI object.
 */
CefBrowserHost* GetCefFromJNIObject(JNIEnv* env, jobject jBrowser);

/**
 * Returns the Java field for the JNI object.
 */
jobject GetJavaField(JNIEnv* env, jobject obj, std::string field, std::string sig);

/**
 * Callback to Java to log the message.
 */
void Log(JNIEnv* env, std::string msg);

#endif
