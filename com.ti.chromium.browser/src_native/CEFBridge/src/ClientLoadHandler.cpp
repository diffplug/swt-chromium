#include "ClientLoadHandler.h"
#include "JNIUtilities.h"

ClientLoadHandler::ClientLoadHandler(jobject& jBrowser) :  m_jBrowser(jBrowser) {
}

void ClientLoadHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
}

void ClientLoadHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
/*
	JNIEnv* jniEnv = GetJNIEnv();
	CefString url = frame->GetURL();
	bool isMain = frame->IsMain();

	Log(jniEnv, "ClientLoadHandler::OnLoadEnd() - " + url.ToString());

	jobject field = GetJavaField(jniEnv, m_jBrowser, "N_LocationListener", "Lcom/ti/chromium/browser/internal/cef/N_LocationListener;");
	if (field != NULL) {
		jclass clazz = jniEnv->GetObjectClass(field);
		jmethodID methodId = jniEnv->GetMethodID(clazz, "changed", "(Ljava/lang/String;Z)V");

		if (methodId != 0) {
			jniEnv->CallVoidMethod(field, methodId, CreateJNIString(jniEnv, url), isMain);

		} else {
			jniEnv->ExceptionClear();
		}
	
	} else {
		jniEnv->ExceptionClear();
	}
*/
}
