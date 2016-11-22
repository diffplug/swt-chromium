#include "JNIUtilities.h"
#include <include/cef_task.h>

#include <map>

#define CEF_HANDLE "N_CefHandle"

extern bool g_LoggingEnabled;
extern jobject g_JavaLogger;
extern JavaVM* g_JavaVM;
extern std::map<int, JNIEnv*> g_JNIEnv;

JNIEnv* attachJNIThread(int threadId) {
	if (g_JNIEnv[threadId] == NULL) {
		JNIEnv* env = 0;
		if (g_JavaVM && g_JavaVM->AttachCurrentThread((void**)&env, NULL) == 0) {
			g_JNIEnv[threadId] = env;
			return g_JNIEnv[threadId];
		}
		return NULL;

	} else {
		return g_JNIEnv[threadId];
	}
}

JNIEnv* GetJNIEnv() {
	if (CefCurrentlyOn(TID_UI)) {
		return attachJNIThread(TID_UI);
	} else if(CefCurrentlyOn(TID_IO)) {
		return attachJNIThread(TID_IO);
	} else if(CefCurrentlyOn(TID_FILE)) {
		return attachJNIThread(TID_FILE);
	}
  
	return NULL;
}

CefString GetStringFromJNI(JNIEnv* env, jstring jstr) {
	CefString wstr;
	const char* chr = NULL;
	if (jstr)
		chr = env->GetStringUTFChars(jstr, NULL);
	if (chr)
		wstr = chr;
	env->ReleaseStringUTFChars(jstr, chr);
	return wstr;
}

jstring CreateJNIString(JNIEnv* env, const CefString& str) {
	std::string cstr(str);
	return env->NewStringUTF(cstr.c_str());
}

bool SetCefForJNIObject(JNIEnv* env, jobject obj, CefBrowserHost* base) {
	jclass clazz = env->GetObjectClass(obj);
	if (clazz == 0) {
		env->ExceptionClear();
		return false;
	}

	jfieldID fieldId = env->GetFieldID(clazz, CEF_HANDLE, "J");
	if (fieldId == 0) {
		env->ExceptionClear();
		return false;
	}

	CefBrowserHost* oldbase = reinterpret_cast<CefBrowserHost*>(env->GetLongField(obj, fieldId));
	if (oldbase) {
		// Remove a reference from the previous base object.
		oldbase->Release();
	}

	env->SetLongField(obj, fieldId, (jlong)base);
	if (base) {
		// Add a reference to the new base object.
		base->AddRef();
	}
	return true;
}

CefBrowserHost* GetCefFromJNIObject(JNIEnv* env, jobject jBrowser) {
	jclass clazz = env->GetObjectClass(jBrowser);
	if (clazz == 0) {
		env->ExceptionClear();
		return NULL;
	}

	jfieldID handleID = env->GetFieldID(clazz, CEF_HANDLE, "J");
	if (handleID == 0) {
		env->ExceptionClear();
		return NULL;
	}
	return reinterpret_cast<CefBrowserHost*>(env->GetLongField(jBrowser, handleID));
}

jobject GetJavaField(JNIEnv* env, jobject obj, std::string field, std::string sig) {
	jclass clazz = env->GetObjectClass(obj);
	if (clazz != 0) {
		jfieldID fieldId = env->GetFieldID(clazz, field.c_str(), sig.c_str());
		if (fieldId != 0) {
			return env->GetObjectField(obj, fieldId);
		} else {
			env->ExceptionClear();
		}
	} else {
		env->ExceptionClear();
	}

	return NULL;
}

void Log(JNIEnv* env, std::string msg) {
	try {
		if (g_LoggingEnabled != TRUE)
			return;

		if (g_JavaLogger != NULL) {
			jclass clazz = env->GetObjectClass(g_JavaLogger);
			if (clazz != 0) {
				jmethodID methodId = env->GetMethodID(clazz, "log", "(Ljava/lang/String;)V");
				if (methodId != 0) {
					env->CallVoidMethod(g_JavaLogger, methodId, env->NewStringUTF(msg.c_str()));
				} else {
					env->ExceptionClear();
				}
			} else {
				env->ExceptionClear();
			}
		}
	} catch (...) {}
}
