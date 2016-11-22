#ifndef _CEF_CLIENT_APP_H_
#define _CEF_CLIENT_APP_H_
#pragma once

#include <include/cef_app.h>

class ClientApp : public CefApp {
public:
	ClientApp(CefBrowserProcessHandler* browserProcessHandler);
	
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE {return m_browserProcessHandler;};

private:
	/* CefApp methods */
	IMPLEMENT_REFCOUNTING(ClientApp);

	/* Cef handlers */
	CefRefPtr<CefBrowserProcessHandler> m_browserProcessHandler;
};

#endif
