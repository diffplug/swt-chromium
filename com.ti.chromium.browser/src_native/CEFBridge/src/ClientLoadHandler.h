#ifndef _CEF_CLIENT_LOAD_HANDLER_H_
#define _CEF_CLIENT_LOAD_HANDLER_H_
#pragma once

#include <jni.h>
#include <include/cef_client.h>

class ClientLoadHandler : public CefLoadHandler {
public:
	ClientLoadHandler(jobject& jBrowser);

private:
	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(ClientLoadHandle);

	/* Java-JNI members */
	jobject& m_jBrowser;
};
#endif
