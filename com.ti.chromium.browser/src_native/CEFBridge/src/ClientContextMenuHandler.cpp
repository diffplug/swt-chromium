#include "ClientContextMenuHandler.h"
#include "JNIUtilities.h"

ClientContextMenuHandler::ClientContextMenuHandler(jobject& jBrowser, std::map<int, MENU_INFO>& menu) : 
	m_jBrowser(jBrowser) ,
	m_menu(menu) {
}


void ClientContextMenuHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, 
													CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) {
	Log(GetJNIEnv(), "ClientContextMenuHandler::OnBeforeContextMenu()");
	model->Clear();
	
	std::map<int, MENU_INFO>::iterator it = m_menu.begin();
	while (it != m_menu.end()) {
		model->AddItem((*it).first, (*it).second.label);
		it++;
	}

}

bool ClientContextMenuHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, 
													CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) {
	JNIEnv* jniEnv = GetJNIEnv();
	Log(jniEnv, "ClientContextMenuHandler::OnContextMenuCommand()");
	std::map<int, MENU_INFO>::iterator it = m_menu.find(command_id);
	if (it != m_menu.end()) {

		jclass cls = jniEnv->GetObjectClass(m_jBrowser);
		jmethodID methodId = jniEnv->GetMethodID(cls, "onContextMenu", "(II)V");
		if (methodId != 0) {
			jniEnv->CallVoidMethod(m_jBrowser, methodId, command_id, params->GetTypeFlags());
		}
	}
	return true;
}
