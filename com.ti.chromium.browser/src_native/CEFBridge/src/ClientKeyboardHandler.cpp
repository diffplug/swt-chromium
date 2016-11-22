#include "ClientKeyboardHandler.h"
#include "JNIUtilities.h"

ClientKeyboardHandler::ClientKeyboardHandler(jobject& jBrowser) :  m_jBrowser(jBrowser) {
}

bool ClientKeyboardHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut) {
	if (os_event != NULL) {
		JNIEnv* jniEnv = GetJNIEnv();
		Log(jniEnv, "ClientKeyboardHandler::OnPreKeyEvent()");

		jobject field = GetJavaField(jniEnv, m_jBrowser, "N_KeyboardListener", "Lcom/ti/chromium/browser/internal/cef/N_KeyboardListener;");
		if (field != NULL) {
			jclass clazz = jniEnv->GetObjectClass(field);
			jmethodID methodId = jniEnv->GetMethodID(clazz, "onKeyEvent", "(III)Z");
			if (methodId != 0) {
#if defined(WIN32)
				if (jniEnv->CallBooleanMethod(field, methodId, os_event->message, os_event->wParam, os_event->lParam)) {
					return true;
				}
#elif defined(linux)
				if (jniEnv->CallBooleanMethod(field, methodId, 0x1b/*(SWT) Widget.KEY_PRESS_EVENT*/, &os_event->key, 0)) {
					return true;
				}
#endif
			}
		}
	}

	return false;
}

bool ClientKeyboardHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event) {
	return false;
}

