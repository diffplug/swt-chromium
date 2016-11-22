#ifndef _CEF_CLIENT_REQUEST_HANDLER_H_
#define _CEF_CLIENT_REQUEST_HANDLER_H_
#pragma once

#include <jni.h>
#include <include/cef_client.h>

class ClientRequestHandler : public CefRequestHandler {
public:
	ClientRequestHandler(jobject& jBrowser);

	bool NotifyURLChanging(CefString url, bool isMain);
	void NotifyURLChanged(CefString url, bool isMain);
private:
	IMPLEMENT_REFCOUNTING(ClientRequestHandler);

	/* Java-JNI members */
	jobject& m_jBrowser;
};
#endif
