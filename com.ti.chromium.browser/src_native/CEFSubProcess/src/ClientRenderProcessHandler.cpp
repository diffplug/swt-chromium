#include "ClientRenderProcessHandler.h"
#include "ClientV8Handler.h"
#include "CefV8Utilties.h"
#include "../../Common/ProcessMessage.h"
#include "../../Common/NamedDataPipe.h"

ClientRenderProcessHandler::ClientRenderProcessHandler() {
}

void ClientRenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
	/* Send the context create message */
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(MSG_RENDER_CONTEXT_CREATED);
	browser->SendProcessMessage(PID_BROWSER, msg);
}

void ClientRenderProcessHandler::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
}

bool ClientRenderProcessHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
	CefString msgName = message->GetName();
	CefRefPtr<CefListValue> argList = message->GetArgumentList();

	/* Expression evaluation */
	if (msgName == MSG_EVAL) {
		/* evaluate the message */
		CefRefPtr<CefV8Context> v8Context = browser->GetMainFrame()->GetV8Context();
		CefRefPtr<CefV8Value> retval = CefV8Value::CreateUndefined();
		CefRefPtr<CefV8Exception> exception;
		bool ret = v8Context->Eval(message->GetArgumentList()->GetString(MSG_EVAL_INDEX), retval, exception);

		CefRefPtr<CefProcessMessage> response = CefProcessMessage::Create(MSG_EVAL_RESPOND);
		CefRefPtr<CefListValue> list = response->GetArgumentList();
		
		if (ret && retval) {
			SetListValue(list, MSG_EVAL_RESPOND_RESULT_INDEX, retval);

		} else if (exception) {
			list->SetString(MSG_EVAL_RESPOND_EXCEPTION_INDEX, exception->GetMessage());
		}

		/* Send the result back to the render process */
		browser->SendProcessMessage(PID_BROWSER, response);

		return true;
	
	/* Add browser function */
	} else if (msgName == MSG_ADD_FUNCTION) {
		CefString funcName = argList->GetString(MSG_ADD_FUNCTION_NAME_INDEX);
		//m_functions.push_back(funcName);

		CefRefPtr<CefV8Context> v8Context = browser->GetMainFrame()->GetV8Context();
		v8Context->Enter();
		CefRefPtr<ClientV8Handler> handler = new ClientV8Handler();
		CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(funcName, static_cast<CefRefPtr<CefV8Handler> >(handler));
		v8Context->GetGlobal()->SetValue(funcName, func, V8_PROPERTY_ATTRIBUTE_NONE);
		v8Context->Exit();

		return true;
	
	/* Remove browser function */
	} else if (msgName == MSG_REMOVE_FUNCTION) {
		// TODO:
	}

	return false;
}

 bool ClientRenderProcessHandler::OnBeforeNavigation(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, NavigationType navigation_type, bool is_redirect) {
	CefString url = request->GetURL();

	/* Create a request */
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(MSG_BEFORE_NAVIGATE);
	CefRefPtr<CefListValue> list = msg->GetArgumentList();
	
	/* Set the url */
	list->SetString(MSG_BEFORE_NAVIGATE_INDEX, url);

	/* Create the pipe */
	NamedDataPipe pipe(IPC_NAVIGATION_PIPE_NAME, true);

	/* Send the request */
	if (browser->SendProcessMessage(PID_BROWSER, msg)) {

		/* Get the result from the pipe */
		std::string result = pipe.getResult();
		return result == CEF_FAILED;
	}

	return false; 
 }
