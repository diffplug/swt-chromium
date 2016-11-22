#include "N_Cef.h"
#include "ClientApp.h"
#include "ClientBrowserProcessHandler.h"
#include "JNIUtilities.h"
#include "../../Common/JSONUtilities.h"

#include <jni.h>
#include <include/cef_app.h>
#include <include/cef_task.h>

/*
 * Enable logging
 */
bool g_LoggingEnabled;

/*
 * CEF settings
 */
Json::Value g_AppSettings;

/*
 * Java callback logger object
 */
jobject g_JavaLogger;

/*
 * Pointer to the JVM
 */
JavaVM* g_JavaVM;

/*
 * A map to the JNI enviornment object per thread
 */
std::map<int, JNIEnv*> g_JNIEnv;

/*
 * Initialize the CEF settings object with the values in the global JSON object
 */
void InitCefSettings(CefSettings& settings) {
	CefString(&settings.cache_path) = g_AppSettings["cache_path"].asString();
	CefString(&settings.user_agent) = g_AppSettings["user_agent"].asString();
	CefString(&settings.product_version) = g_AppSettings["product_version"].asString();
	CefString(&settings.locale) = g_AppSettings["locale"].asString();
	settings.remote_debugging_port = g_AppSettings["remote_debugging_port"].asInt();

	switch (g_AppSettings["log_severity"].asInt()) {
	case 0:
		settings.log_severity = LOGSEVERITY_DEFAULT;
		break;
	case 1:
		settings.log_severity = LOGSEVERITY_VERBOSE;
		break;
	case 2:
		settings.log_severity = LOGSEVERITY_INFO;
		break;
	case 3:
		settings.log_severity = LOGSEVERITY_WARNING;
		break;
	case 4:
		settings.log_severity = LOGSEVERITY_ERROR;
		break;
	case 5:
		settings.log_severity = LOGSEVERITY_ERROR_REPORT;
		break;
	case 99:
		settings.log_severity = LOGSEVERITY_DISABLE;
		break;
	default:
		settings.log_severity = LOGSEVERITY_DEFAULT;
	}
	
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1Cef_N_1EnableLogging(JNIEnv* env, jobject cef, jboolean enabled) {
	g_LoggingEnabled = (enabled == JNI_TRUE ? true : false);
}

JNIEXPORT jboolean JNICALL Java_com_ti_chromium_browser_cef_N_1Cef_N_1Initialize(JNIEnv* env, jobject cef, jstring jAppSettings, jstring jLibDir) {
	/*
	 * Get the JVM pointer
	 */
	env->GetJavaVM(&g_JavaVM);

	/*
	 * Get the Java logger from the cef object
	 */
	try {
		g_JavaLogger = GetJavaField(env, cef, "N_JavaLogger", "Lcom/ti/chromium/browser/internal/cef/N_JavaLogger;");
		g_JavaLogger = env->NewGlobalRef(g_JavaLogger);
	} catch (...) {}
	Log(env, "Cef::Initialize()");

	/*
	 * Create a JSON object from the CefSettings string
	 */
	g_AppSettings = GetJsonValue(GetStringFromJNI(env, jAppSettings));

	/*
	 * Initialize the CefSettings
	 */
	CefSettings settings;
	InitCefSettings(settings);
	Log(env, "\tInitializing CefSettings parameters");
	
	CefString libDir = GetStringFromJNI(env, jLibDir);

	CefString(&settings.resources_dir_path) = libDir;
	CefString(&settings.locales_dir_path) = CefString(libDir.ToString() + "locales/");

#if defined(WIN32)
	CefString(&settings.browser_subprocess_path) = CefString(libDir.ToString() + "cef_sub_process.exe");
#elif  defined(linux)
	CefString(&settings.browser_subprocess_path) = CefString(libDir.ToString() + "cef_sub_process");
#endif

	/*
	 * Create a client application callback handler
	 */
	CefRefPtr<ClientBrowserProcessHandler> handler = new ClientBrowserProcessHandler(&settings.browser_subprocess_path);
	CefRefPtr<ClientApp> clientApp = new ClientApp(handler);

#if defined(WIN32)
	HINSTANCE hInstance = GetModuleHandle(NULL);
	CefMainArgs args(hInstance);
#elif defined(linux)
	CefMainArgs args(0, NULL);
#endif

	/*
	 * Initialize the CEF runtime
	 */
	Log(env, "\tInitializing CEF runtime");
	void* sandbox_info = NULL;

#if CEF_ENABLE_SANDBOX
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	if (!CefInitialize(args, settings, clientApp.get(), sandbox_info)) {
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1Cef_N_1Shutdown(JNIEnv* env, jobject) {
	Log(env, "Cef::Shutdown()");

	/*
	 * Delete the java logger reference
	 */
	env->DeleteGlobalRef(g_JavaLogger);

	/*
	 * Shutdown CEF runtime
	 */
	CefShutdown();
}

JNIEXPORT void JNICALL Java_com_ti_chromium_browser_cef_N_1Cef_N_1DoMessageLoopWork(JNIEnv* env, jobject) {
	//Log(env, "Cef.DoMessageLoopWork()");
	CefDoMessageLoopWork();
}
