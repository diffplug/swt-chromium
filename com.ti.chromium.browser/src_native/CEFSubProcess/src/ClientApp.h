#ifndef CLIENTAPP_H_
#define CLIENTAPP_H_

#include <include/cef_app.h>

class ClientApp : public CefApp {
public:
	ClientApp(CefRenderProcessHandler* renderProcessHandler);

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {return m_renderProcessHandler;};

private:
	/* Cef handlers */
	CefRefPtr<CefRenderProcessHandler> m_renderProcessHandler;

	/* CefApp methods */
	IMPLEMENT_REFCOUNTING(ClientApp);
};

#endif /* CLIENTAPP_H_ */
