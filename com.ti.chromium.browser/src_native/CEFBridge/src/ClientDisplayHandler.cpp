#include "ClientDisplayHandler.h"
#include "ClientHandler.h"

ClientDisplayHandler::ClientDisplayHandler(ClientHandler& clientHandler, jobject& jBrowser) : m_clientHandler(clientHandler), m_jBrowser(jBrowser) {
}

void ClientDisplayHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) {
	m_clientHandler.OnAddressChange(browser, frame, url);
}

void ClientDisplayHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
  CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();

#if defined(WIN32)
  SetWindowTextW(hwnd, title.c_str());
#elif defined(linux)
  GdkWindow *gtk_window = gtk_widget_get_window(GTK_WIDGET(hwnd));
  gdk_window_set_title(gtk_window, title.ToString().c_str());
#endif
}
