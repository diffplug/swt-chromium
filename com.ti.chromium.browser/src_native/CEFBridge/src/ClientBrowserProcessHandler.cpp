#include "ClientBrowserProcessHandler.h"

ClientBrowserProcessHandler::ClientBrowserProcessHandler(CefString subProcessPath):
	m_subProcessPath(subProcessPath) {
}

void ClientBrowserProcessHandler::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) {

	/* TODO: workaround CEF3 bug (https://code.google.com/p/chromiumembedded/issues/detail?id=980) */
	const std::string& type = command_line->GetSwitchValue("type");
	if (type == "zygote")  {
		command_line->SetProgram(m_subProcessPath);
	}
}
