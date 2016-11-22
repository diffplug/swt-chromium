#include "ClientV8Handler.h"
#include "CefV8Utilties.h"
#include "../../Common/JSONUtilities.h"
#include "../../Common/ProcessMessage.h"
#include "../../Common/NamedDataPipe.h"

ClientV8Handler::ClientV8Handler() {
}

bool ClientV8Handler::Execute(const CefString& name, CefRefPtr<CefV8Value> object,
                       const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {					   

	/* Create a request */
	CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(MSG_FUNCTION_CALLBACK);
	CefRefPtr<CefListValue> argList = message->GetArgumentList();

	/* Set the function name */
	argList->SetString(MSG_FUNCTION_CALLBACK_FUNC_NAME_INDEX, name);
	
	/* Set the function arguments */
	CefRefPtr<CefListValue> args = CefListValue::Create();
	for (int i = 0; i < (int) arguments.size(); ++i) {
		SetListValue(args, i, arguments[i]);
	}
	argList->SetList(MSG_FUNCTION_CALLBACK_FUNC_ARGS_INDEX, args);

	/* Create the pipe */
	NamedDataPipe dataPipe(IPC_V8_PIPE_NAME, true);
	
	/* Send the request */
	CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
	if (browser->SendProcessMessage(PID_BROWSER, message)) {

		/* Get the result from the pipe */
		std::string result = dataPipe.getResult();

		/* Return the JSON object back to V8 */
		retval = CreateV8ValueFromJSON(GetJsonValue(result)[JSON_ROOT]);
	}

	return true;
}
