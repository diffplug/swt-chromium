#ifndef _CEF_CLIENT_DISPLAY_HANDLER_H_
#define _CEF_CLIENT_DISPLAY_HANDLER_H_
#pragma once

#include <jni.h>
#include <include/cef_client.h>

class ClientHandler;

class ClientDisplayHandler : public CefDisplayHandler {
public:
	ClientDisplayHandler(ClientHandler& clientHandler, jobject& jBrowser);

private:
	virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               const CefString& url) OVERRIDE;

	void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(ClientDisplayHandler);

	ClientHandler& m_clientHandler;

	/* Java-JNI members */
	jobject& m_jBrowser;
};

#endif
