#include "N_CefBrowser.h"
#include "ClientHandler.h"
#include "JNIUtilities.h"
#include "../../Common/ProcessMessage.h"

#include <jni.h>

#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_task.h"

class SetURLTask : public CefTask {
public:
	SetURLTask(CefRefPtr<CefFrame> frame, CefString url) {
		m_frame = frame;
		m_url = url;
	}

	virtual void Execute(CefThreadId threadId) {
		REQUIRE_UI_THREAD()
		m_frame->LoadURL(m_url);
	}

	IMPLEMENT_REFCOUNTING(OnBeforeResourceLoadTask);
	CefRefPtr<CefFrame> m_frame;
	CefString m_url;
};


JNIEXPORT jboolean JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1CreateBrowser(JNIEnv* env, jobject jBrowser, jint jhParentWnd, jstring jUrl) {
	Log(env, "CefBrowser::CreateBrowser()");

#if defined(WIN32)
	/* Set the parent window handle and initial rect size */
	RECT rect;
	rect.top = rect.left = rect.bottom = rect.right = 0;
	CefWindowInfo info;	
	info.SetAsChild((HWND)jhParentWnd, rect);
#elif defined(linux)
	CefWindowInfo info;	
	info.SetAsChild((CefWindowHandle)jhParentWnd);
#endif


	/* Client callback handle */
	CefRefPtr<ClientHandler> handle = new ClientHandler(env, jBrowser);

	/* Default url */
	CefString url = GetStringFromJNI(env, jUrl);
	
	/* Create the browser */
	CefBrowserSettings browserSettings;
	browserSettings.file_access_from_file_urls = STATE_ENABLED;
	browserSettings.universal_access_from_file_urls = STATE_ENABLED;

	CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(info, handle.get(), url, browserSettings, NULL);
	SetCefForJNIObject(env, jBrowser, browser->GetHost());
	handle->SetBrowser(browser);

	return JNI_TRUE;
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1Destroy(JNIEnv* env, jobject jBrowser) {
	Log(env, "CefBrowser::Destroy()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	CefRefPtr<ClientHandler> handler = dynamic_cast<ClientHandler*> (browserHost->GetClient().get());
	handler->ReleaseResources();

	browserHost->CloseBrowser(true);
	SetCefForJNIObject(env, jBrowser, NULL);
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1SetWindowSize(JNIEnv* env, jobject jBrowser, jint jX, jint jY, jint jWidth, jint jHeight) {
	Log(env, "CefBrowser::SetWindowSize()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	CefWindowHandle hwnd = browserHost.get()->GetWindowHandle();
	if (hwnd) {

#if defined(WIN32)
		SetWindowPos(hwnd, NULL, jX, jY, jWidth, jHeight, SWP_NOZORDER);
#elif defined(linux)
		GtkAllocation alloc;;
		alloc.x = jX;
		alloc.y = jY;
		alloc.width = jWidth;
		alloc.height = jHeight;
		gtk_widget_size_allocate(GTK_WIDGET(hwnd), &alloc);
#endif
	}
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1SetUrl(JNIEnv* env, jobject jBrowser, jstring jUrl) {
	CefString url = GetStringFromJNI(env, jUrl);

	Log(env, "CefBrowser::SetUrl() - " + std::string(url));

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	CefRefPtr<CefFrame> frame = browserHost->GetBrowser()->GetMainFrame();
	
	/* 
	 * Calling LoadURL can cause a deadlock in the CEF code.
	 * I am not able to figure out the root cause, but calling
	 * LoadRequest seems to prevent the deadlock for remote URL.
	 */
	/*std::string httpPrefix = std::string("http");
	if (!std::string(url).compare(0, httpPrefix.size(), httpPrefix)) {
		Log(env, "CefBrowser::SetUrl() - LoadRequest");
		CefRefPtr<CefRequest> request(CefRequest::Create());
		request->SetURL(url);
		frame->LoadRequest(request);

	} else {
		Log(env, "CefBrowser::SetUrl() - LoadURL");
		std::string sURL = url;
		frame->LoadURL(sURL);
	}*/

	/*
	 * This workaround seems to work without calling LoadRequest
	 */
	//CefRefPtr<SetURLTask> task(new SetURLTask(frame, url));
	//CefPostTask(TID_UI, static_cast<CefRefPtr<CefTask>>(task));
	
	// TODO: after fixing the deadlock in the libcef.dll, loadURL no longer an issue.
	//		 Keeping the existing code for reference.
	frame->LoadURL(std::string(url));
}

JNIEXPORT jstring JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1GetUrl(JNIEnv* env, jobject jBrowser) {
	Log(env, "CefBrowser::GetUrl()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return NULL;

	return CreateJNIString(env, browserHost->GetBrowser()->GetMainFrame()->GetURL());
}

JNIEXPORT jboolean JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1IsBackEnabled(JNIEnv* env, jobject jBrowser) {
	Log(env, "CefBrowser::IsBackEnabled()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return JNI_FALSE;

	return browserHost->GetBrowser()->CanGoBack();
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1Back(JNIEnv* env, jobject jBrowser) {
	Log(env, "CefBrowser::Back()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	browserHost->GetBrowser()->GoBack();
}

JNIEXPORT jboolean JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1IsForwardEnabled(JNIEnv* env, jobject jBrowser) {
	Log(env, "CefBrowser::IsForwardEnabled()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return JNI_FALSE;

	return browserHost->GetBrowser()->CanGoForward();
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1Forward(JNIEnv* env, jobject jBrowser) {
	Log(env, "CefBrowser::Forward()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	browserHost->GetBrowser()->GoForward();
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1Refresh(JNIEnv* env, jobject jBrowser) {
	Log(env, "CefBrowser::Refresh()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	browserHost->GetBrowser()->Reload();
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1Stop(JNIEnv* env, jobject jBrowser) {
	Log(env, "CefBrowser::Stop()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	browserHost->GetBrowser()->StopLoad();
}

JNIEXPORT jboolean JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1Execute(JNIEnv* env, jobject jBrowser, jstring jCode) {
	Log(env, "CefBrowser::Execute()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return JNI_FALSE;

	browserHost->GetBrowser()->GetMainFrame()->ExecuteJavaScript(GetStringFromJNI(env, jCode), "about:blank", 0);

	return JNI_TRUE;
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1Evaluate(JNIEnv* env, jobject jBrowser, jstring jCode) {
	Log(env, "CefBrowser::Evaluate");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(MSG_EVAL);
	message->GetArgumentList()->SetString(MSG_EVAL_INDEX, GetStringFromJNI(env, jCode));
	browserHost->GetBrowser()->SendProcessMessage(PID_RENDERER, message);
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1CreateBrowserFunction(JNIEnv* env, jobject jBrowser, jstring jFunctionName, jobject jFunction) {
	CefString functionName = GetStringFromJNI(env, jFunctionName);
	Log(env, "CefBrowser::CreateBrowserFunction() - " + std::string(functionName));

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	CefRefPtr<ClientHandler> handler = dynamic_cast<ClientHandler*> (browserHost->GetClient().get());
	handler->RegisterBrowserFunction(functionName, jFunction);
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1DestroyBrowserFunction(JNIEnv* env, jobject jBrowser, jstring jFunctionName) {
	CefString functionName = GetStringFromJNI(env, jFunctionName);
	Log(env, "CefBrowser::DestroyBrowserFunction() - " + std::string(functionName));

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	CefRefPtr<ClientHandler> handler = dynamic_cast<ClientHandler*> (browserHost->GetClient().get());
	handler->UnregisterBrowserFunction(functionName);
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1AddContextMenu(JNIEnv* env, jobject jBrowser, jint jId, jstring jLabel, jobject jMenu) {
	CefString label = GetStringFromJNI(env, jLabel);
	Log(env, "CefBrowser::AddContextMenu() - " + std::string(label));

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	CefRefPtr<ClientHandler> handler = dynamic_cast<ClientHandler*> (browserHost->GetClient().get());
	handler->AddContextMenu(jId, label, jMenu);
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1RemoveContextMenu(JNIEnv* env, jobject jBrowser, jint jId) {
	Log(env, "CefBrowser::RemoveContextMenu()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	CefRefPtr<ClientHandler> handler = dynamic_cast<ClientHandler*> (browserHost->GetClient().get());
	handler->RemoveContextMenu(jId);
}

JNIEXPORT jdouble JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1GetZoomLevel(JNIEnv* env, jobject jBrowser) {
	Log(env, "CefBrowser::GetZoomLevel()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return 1;

	return browserHost->GetZoomLevel();
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1CefBrowser_N_1SetZoomLevel(JNIEnv* env, jobject jBrowser, jdouble level) {
	Log(env, "CefBrowser::SetZoomLevel()");

	CefRefPtr<CefBrowserHost> browserHost(GetCefFromJNIObject(env, jBrowser));
	if (!browserHost.get())
		return;

	browserHost->SetZoomLevel(level);
}