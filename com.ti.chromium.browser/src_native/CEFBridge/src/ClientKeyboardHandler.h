#ifndef _CEF_CLIENT_KEYBOARD_HANDLER_H_
#define _CEF_CLIENT_KEYBOARD_HANDLER_H_
#pragma once

#include <jni.h>
#include <include/cef_client.h>

class ClientKeyboardHandler : public CefKeyboardHandler {
public:
	ClientKeyboardHandler(jobject& jBrowser);

private:
	virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                             const CefKeyEvent& event,
                             CefEventHandle os_event,
                             bool* is_keyboard_shortcut) OVERRIDE;
	
	virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
                          const CefKeyEvent& event,
                          CefEventHandle os_event) OVERRIDE;
private:
	IMPLEMENT_REFCOUNTING(ClientKeyboardHandler);

	/* Java-JNI members */
	jobject& m_jBrowser;
};
#endif
