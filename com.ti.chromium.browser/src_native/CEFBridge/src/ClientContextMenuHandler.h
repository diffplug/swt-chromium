#ifndef _CEF_CLIENT_CONTEXT_MENU_HANDLE_H_
#define _CEF_CLIENT_CONTEXT_MENU_HANDLE_H_
#pragma once

#include <include/cef_client.h>
#include <jni.h>

struct MENU_INFO {
	CefString label;
	jobject menu;
};


class ClientContextMenuHandler : public CefContextMenuHandler {
public:
	ClientContextMenuHandler(jobject& jBrowser, std::map<int, MENU_INFO>& menu);

private:
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model) OVERRIDE;
	
	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefContextMenuParams> params,
                                    int command_id,
                                    EventFlags event_flags) OVERRIDE;
private:
	/* ClientContextMenuHandler methods */
	IMPLEMENT_REFCOUNTING(ClientContextMenuHandler);

	/* Java-JNI members */
	jobject& m_jBrowser;

	std::map<int, MENU_INFO>& m_menu;
};

#endif
