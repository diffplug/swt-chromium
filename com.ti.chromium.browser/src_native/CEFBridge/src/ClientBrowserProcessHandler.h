#ifndef _CEF_CLIENT_BROWSER_PROCESS_HANDLER_H_
#define _CEF_CLIENT_BROWSER_PROCESS_HANDLER_H_
#pragma once

#include <include/cef_app.h>

class ClientBrowserProcessHandler : public CefBrowserProcessHandler {
public:
	ClientBrowserProcessHandler(CefString subProcessPath);

private:
	virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) OVERRIDE;
	
	CefString m_subProcessPath;

	/* ClientBrowserProcessHandler methods */
	IMPLEMENT_REFCOUNTING(ClientBrowserProcessHandler);
};

#endif
