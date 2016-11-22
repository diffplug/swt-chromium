#ifndef _CEF_CLIENT_DOWNLOAD_HANDLER_H_
#define _CEF_CLIENT_DOWNLOAD_HANDLER_H_
#pragma once

#include <jni.h>
#include <include/cef_client.h>

class ClientDownloadHandler : public CefDownloadHandler {
public:
	ClientDownloadHandler(jobject& jBrowser);

private:
	  virtual void OnBeforeDownload(CefRefPtr<CefBrowser> browser,
									CefRefPtr<CefDownloadItem> download_item,
									const CefString& suggested_name,
									CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(ClientDownloadHandler);

	/* Java-JNI members */
	jobject& m_jBrowser;
};

#endif
