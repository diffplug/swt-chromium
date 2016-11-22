#ifndef CLIENTRANDERPROCESSHANDLER_H_
#define CLIENTRANDERPROCESSHANDLER_H_

#include <list>
#include <include/cef_app.h>

class ClientRenderProcessHandler : public CefRenderProcessHandler {
public:
	ClientRenderProcessHandler();

private:
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
								CefRefPtr<CefFrame> frame,
								CefRefPtr<CefV8Context> context) OVERRIDE;

	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
								CefRefPtr<CefFrame> frame,
								CefRefPtr<CefV8Context> context) OVERRIDE;

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
										CefProcessId source_process,
										CefRefPtr<CefProcessMessage> message) OVERRIDE;

	virtual bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefRequest> request,
                                  NavigationType navigation_type,
                                  bool is_redirect) OVERRIDE;

private:
	CefRefPtr<CefV8Context> m_context;
	std::list<CefString> m_functions;

	IMPLEMENT_REFCOUNTING(ClientRenderProcessHandler);
};

#endif /* CLIENTRANDERPROCESSHANDLER_H_ */
