#ifndef _CEF_CLIENT_V8_HANDLER_H_
#define _CEF_CLIENT_V8_HANDLER_H_
#pragma once

#include <include/cef_app.h>

class ClientV8Handler : public CefV8Handler {
public:
	ClientV8Handler();

private:
	virtual bool Execute(const CefString& name,
						CefRefPtr<CefV8Value> object,
						const CefV8ValueList& arguments,
						CefRefPtr<CefV8Value>& retval,
						CefString& exception) OVERRIDE;

	IMPLEMENT_REFCOUNTING(ClientV8Handler);
};
#endif
