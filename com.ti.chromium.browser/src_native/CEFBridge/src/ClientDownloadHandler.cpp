#include "ClientDownloadHandler.h"

ClientDownloadHandler::ClientDownloadHandler(jobject& jBrowser) : m_jBrowser(jBrowser) {
}

void ClientDownloadHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item,
									const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback) {

	callback->Continue(CefString(), true);
}