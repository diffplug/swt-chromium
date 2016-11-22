#ifndef _CEF_CLIENT_HANDLE_H_
#define _CEF_CLIENT_HANDLE_H_
#pragma once

#include "ClientContextMenuHandler.h"
#include "ClientLoadHandler.h"
#include "ClientRequestHandler.h"
#include "ClientLifeSpanHandler.h"
#include "ClientDisplayHandler.h"
#include "ClientDownloadHandler.h"
#include "ClientDragHandler.h"
#include "ClientKeyboardHandler.h"

#include <jni.h>
#include <include/cef_client.h>

class ClientHandler : public CefClient {
public:
	ClientHandler(JNIEnv* env, jobject jBrowser);
	void ReleaseResources();

	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE {return m_contextMenuHandler;}
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {return m_lifeSpanHandler;}
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {return m_loadHandler;}
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE {return m_requestHandler;}
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {return m_displayHandler;}
	virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE {return m_downloadHandler;}
	virtual CefRefPtr<CefDragHandler> GetDragHandler() {return m_dragHandler;}
	virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() {return m_keyboardHandler;}

	void SetBrowser(CefRefPtr<CefBrowser> browser) {m_browser = browser;}

	void RegisterBrowserFunction(CefString name, jobject function);
	void UnregisterBrowserFunction(CefString  name);

	void AddContextMenu(int id, CefString label, jobject menu);
	void RemoveContextMenu(int id);

	/* callback function for ClientDisplayHandler */
	void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url);
private:
	/* CefClient methods */
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
										  CefProcessId source_process,
                                          CefRefPtr<CefProcessMessage> message) OVERRIDE;


	/* ClientHandler methods */
	void SendFunctionCallbackResult(const char* buffer, int length);
	void DoRegisterBrowserFunctions();

	/* Java-JNI members */
	jobject m_jBrowser;

	/* Delegate class members */
	CefRefPtr<CefContextMenuHandler> m_contextMenuHandler;
	CefRefPtr<CefLoadHandler> m_loadHandler;
	CefRefPtr<CefRequestHandler> m_requestHandler;
	CefRefPtr<CefLifeSpanHandler> m_lifeSpanHandler;
	CefRefPtr<CefDisplayHandler> m_displayHandler;
	CefRefPtr<CefDownloadHandler> m_downloadHandler;
	CefRefPtr<CefDragHandler> m_dragHandler;
	CefRefPtr<CefKeyboardHandler> m_keyboardHandler;
	CefRefPtr<CefBrowser> m_browser;

	std::map<CefString, jobject> m_browserFunctions;
	std::map<int, MENU_INFO> m_contextMenu;


	IMPLEMENT_REFCOUNTING(ClientHandler);
	IMPLEMENT_LOCKING(ClientHandler);
};
#endif
