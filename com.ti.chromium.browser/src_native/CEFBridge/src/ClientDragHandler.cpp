#include "ClientDragHandler.h"

ClientDragHandler::ClientDragHandler(jobject& jBrowser) : m_jBrowser(jBrowser) {
}

bool ClientDragHandler::OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, DragOperationsMask mask) {
	return true;
}