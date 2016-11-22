#include "ClientLifeSpanHandler.h"
#include "JNIUtilities.h"

ClientLifeSpanHandler::ClientLifeSpanHandler(jobject& jBrowser) :  m_jBrowser(jBrowser) {
}

void ClientLifeSpanHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	JNIEnv* jniEnv = GetJNIEnv();
	Log(jniEnv, "ClientLifeSpanHandler::OnAfterCreated()");
}

bool ClientLifeSpanHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             const CefString& target_url,
                             const CefString& target_frame_name,
                             const CefPopupFeatures& popupFeatures,
                             CefWindowInfo& windowInfo,
                             CefRefPtr<CefClient>& client,
                             CefBrowserSettings& settings,
                             bool* no_javascript_access) {

	JNIEnv* jniEnv = GetJNIEnv();
	Log(jniEnv, "ClientLifeSpanHandler::OnBeforePopup() - " + target_url.ToString());

	jobject field = GetJavaField(jniEnv, m_jBrowser, "N_OpenWindowListener", "Lcom/ti/chromium/browser/internal/cef/N_OpenWindowListener;");
	if (field != NULL) {
#if defined(WIN32)
		if (windowInfo.x <= 0)
			windowInfo.x = 0;
		if (windowInfo.y <= 0)
			windowInfo.y = 0;
		if (windowInfo.width <= 0)
			windowInfo.width = 1066;
		if (windowInfo.height <= 0)
			windowInfo.height = 600;
#endif
		jclass clazz = jniEnv->GetObjectClass(field);
		jmethodID methodId = jniEnv->GetMethodID(clazz, "onBeforePopup", "(IIIIZZZZ)Z");

		if (methodId != 0) {
			int x;
			int y;
			int width;
			int height;
#if defined(WIN32)
			x = windowInfo.x;
			y = windowInfo.y;
			width = windowInfo.width;
			height = windowInfo.height;
#elif defined(linux)
			gtk_window_get_position(GTK_WINDOW(windowInfo.parent_widget), &x, &y); 
			gtk_window_get_size(GTK_WINDOW(windowInfo.parent_widget), &width, &height); 
#endif
			if (jniEnv->CallBooleanMethod(field, methodId, 
				CreateJNIString(jniEnv, target_url), 
				x,
				y,
				width,
				height,
				popupFeatures.locationBarVisible,
				popupFeatures.menuBarVisible,
				popupFeatures.statusBarVisible,
				popupFeatures.toolBarVisible)) {
					return true;
			}

		} else {
			jniEnv->ExceptionClear();
		}
	
	} else {
		jniEnv->ExceptionClear();
	}

    return false;
}
