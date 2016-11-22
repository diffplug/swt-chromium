#ifndef _CEF_CLIENT_LIFE_SPAN_HANDLER_H_
#define _CEF_CLIENT_LIFE_SPAN_HANDLER_H_
#pragma once

#include <jni.h>
#include <include/cef_client.h>

class ClientLifeSpanHandler : public CefLifeSpanHandler {
public:
	ClientLifeSpanHandler(jobject& jBrowser);

private:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	
	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             const CefString& target_url,
                             const CefString& target_frame_name,
                             const CefPopupFeatures& popupFeatures,
                             CefWindowInfo& windowInfo,
                             CefRefPtr<CefClient>& client,
                             CefBrowserSettings& settings,
                             bool* no_javascript_access) OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(ClientLifeSpanHandler);

	/* Java-JNI members */
	jobject& m_jBrowser;
};
#endif
