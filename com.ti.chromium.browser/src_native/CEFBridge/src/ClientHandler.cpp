#include "ClientHandler.h"
#include "JNIUtilities.h"
#include "../../Common/JSONUtilities.h"
#include "../../Common/NamedDataPipe.h"
#include "../../Common/ProcessMessage.h"


ClientHandler::ClientHandler(JNIEnv* env, jobject jBrowser) {
	m_jBrowser = env->NewGlobalRef(jBrowser);
	m_contextMenuHandler = new ClientContextMenuHandler(m_jBrowser, m_contextMenu);
	m_lifeSpanHandler = new ClientLifeSpanHandler(m_jBrowser);
	m_loadHandler = new ClientLoadHandler(m_jBrowser);
	m_requestHandler = new ClientRequestHandler(m_jBrowser);
	m_displayHandler = new ClientDisplayHandler(*this, m_jBrowser);
	m_downloadHandler = new ClientDownloadHandler(m_jBrowser);
	m_dragHandler = new ClientDragHandler(m_jBrowser);
	m_keyboardHandler = new ClientKeyboardHandler(m_jBrowser);
}

void ClientHandler::ReleaseResources() {
	GetJNIEnv()->DeleteGlobalRef(m_jBrowser);
	m_browser = NULL;
	m_jBrowser = NULL;
}

bool ClientHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
											 CefProcessId source_process,
											 CefRefPtr<CefProcessMessage> message) {
	try {
		JNIEnv* jniEnv = GetJNIEnv();
		CefString msgName = message->GetName();

		Log(jniEnv, "ClientHandler::OnProcessMessageReceived() " + std::string(msgName));
	
		CefRefPtr<CefListValue> argList = message->GetArgumentList();
		if (msgName == MSG_EVAL_RESPOND) {

			Json::Value result = CreateEvalJSONObject(argList, MSG_EVAL_RESPOND_RESULT_INDEX);
			CefString respond = result.toStyledString();

			if (m_jBrowser) {
				jclass cls = jniEnv->GetObjectClass(m_jBrowser);
				jfieldID fieldId = jniEnv->GetFieldID(cls, "N_EvalRespond", "Ljava/lang/String;");
				if (fieldId != 0) {
					std::string cstr(respond);
					jniEnv->SetObjectField(m_jBrowser, fieldId, jniEnv->NewStringUTF(cstr.c_str()));
				} else {
					jniEnv->ExceptionClear();
				}
			}

			return true;
		} else if (msgName == MSG_RENDER_CONTEXT_CREATED) {
			DoRegisterBrowserFunctions();
			return true;

		} else if (msgName == MSG_FUNCTION_CALLBACK) {
			CefString funcName = argList->GetString(MSG_FUNCTION_CALLBACK_FUNC_NAME_INDEX);
			CefRefPtr<CefListValue> args = argList->GetList(MSG_FUNCTION_CALLBACK_FUNC_ARGS_INDEX);

			/* Call back into JNI to execute the function */
			std::string result("undefined");
			jobject jFunction = m_browserFunctions[funcName];
			if (jFunction) {
				if (m_jBrowser) {
					jclass cls = jniEnv->GetObjectClass(m_jBrowser);
					jmethodID methodId = jniEnv->GetMethodID(cls, "executeFunction", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
					if (methodId != 0) {
						Json::Value root(Json::objectValue);
						root[JSON_ROOT] = CreateJSONArray(args);
						CefString parameters(root.toStyledString());
						jstring funcResult = (jstring) jniEnv->CallObjectMethod(m_jBrowser, methodId, CreateJNIString(jniEnv, funcName), CreateJNIString(jniEnv, parameters));

						/* Create JSON object */
						result = GetJsonValue(GetStringFromJNI(jniEnv, funcResult)).toStyledString();
					} else {
						jniEnv->ExceptionClear();
					}
				}
			}

			/* pass result to the rendering process */
			NamedDataPipe dataPipe(IPC_V8_PIPE_NAME, false);
			dataPipe.setResult(result);

			return true;

		} else if (msgName == MSG_BEFORE_NAVIGATE) {
			CefString url = argList->GetString(MSG_BEFORE_NAVIGATE_INDEX);

			ClientRequestHandler* handler = static_cast<ClientRequestHandler*>(m_requestHandler.get()); 
			bool result = handler->NotifyURLChanging(url, true);

			/* pass result to the rendering process */
			NamedDataPipe pipe(IPC_NAVIGATION_PIPE_NAME, false);
			pipe.setResult(result ? CEF_SUCCEEDED : CEF_FAILED);
		}
	} catch (...) {}

	return false;
}

void ClientHandler::DoRegisterBrowserFunctions() {
	CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(MSG_ADD_FUNCTION);
	CefRefPtr<CefListValue> argList = message->GetArgumentList();
	

	std::map<CefString, jobject>::iterator it = m_browserFunctions.begin();
	for (; it != m_browserFunctions.end(); ++it) {
		argList->SetString(MSG_ADD_FUNCTION_NAME_INDEX, (*it).first);
		m_browser->SendProcessMessage(PID_RENDERER, message);
	}
}

void ClientHandler::RegisterBrowserFunction(CefString name, jobject function) {
	m_browserFunctions[name] = function;
}

void ClientHandler::UnregisterBrowserFunction(CefString name) {
	std::map<CefString, jobject>::iterator it = m_browserFunctions.find(name);
	if (it != m_browserFunctions.end()) {
		m_browserFunctions.erase(it);
		// TODO: send process message to remove function
	}
}

void ClientHandler::AddContextMenu(int id, CefString label, jobject menu) {
	MENU_INFO info;
	info.label = label;
	info.menu = menu;
	m_contextMenu[id] = info;
}

void ClientHandler::RemoveContextMenu(int id) {
	std::map<int, MENU_INFO>::iterator it = m_contextMenu.find(id);
	if (it != m_contextMenu.end()) {
		m_contextMenu.erase(it);
	}
}

void ClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) {
	ClientRequestHandler* handler = static_cast<ClientRequestHandler*>(m_requestHandler.get()); 
	handler->NotifyURLChanged(url, frame.get()->IsMain());
}
