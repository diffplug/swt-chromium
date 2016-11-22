#ifndef _CEF_CLIENT_DRAG_HANDLER_H_
#define _CEF_CLIENT_DRAG_HANDLER_H_
#pragma once

#include <jni.h>
#include <include/cef_client.h>

class ClientDragHandler : public CefDragHandler {
public:
	ClientDragHandler(jobject& jBrowser);


	virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
							CefRefPtr<CefDragData> dragData,
							DragOperationsMask mask) OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(ClientDragHandler);

	/* Java-JNI members */
	jobject& m_jBrowser;
};
#endif
