#include "ClientRequestHandler.h"
#include "JNIUtilities.h"

#if defined(linux)
#include <semaphore.h>
#endif

#include <include/cef_task.h>


class OnBeforeResourceLoadTask : public CefTask {
public:
	jboolean doIt;
	jobject field;
	jmethodID methodId;
	jstring url;
	bool isMain;
#if defined(WIN32)
	HANDLE handle;
#elif defined(linux)
	sem_t handle;
#endif

#if defined(WIN32)
	OnBeforeResourceLoadTask(jobject field, jmethodID methodId, jstring url, bool isMain, HANDLE handle) {
#elif defined(linux)
	OnBeforeResourceLoadTask(jobject field, jmethodID methodId, jstring url, bool isMain, sem_t handle) {
#endif

		this->field = field;
		this->methodId = methodId;
		this->url = url;
		this->isMain = isMain;
		this->handle = handle;
	}

	virtual void Execute() {
		REQUIRE_UI_THREAD()
		doIt = GetJNIEnv()->CallBooleanMethod(field, methodId, url, isMain);
#if defined(WIN32)
		SetEvent(this->handle);
#elif defined(linux)
		sem_post(&handle);
#endif
	}

	IMPLEMENT_REFCOUNTING(OnBeforeResourceLoadTask);
};

ClientRequestHandler::ClientRequestHandler(jobject& jBrowser) :	m_jBrowser(jBrowser) {
}

bool ClientRequestHandler::NotifyURLChanging(CefString url, bool isMain) {
	JNIEnv* jniEnv = GetJNIEnv();
	Log(jniEnv, "ClientRequestHandler::NotifyURLChanging");

	jobject field = GetJavaField(jniEnv, m_jBrowser, "N_LocationListener", "Lcom/ti/chromium/browser/internal/cef/N_LocationListener;");
	if (field != NULL) {
		jclass clazz = jniEnv->GetObjectClass(field);
		jmethodID methodId = jniEnv->GetMethodID(clazz, "changing", "(Ljava/lang/String;Z)Z");

		if (methodId != 0) {
			jboolean doIt = JNI_TRUE;

			if (CefCurrentlyOn(TID_UI)) {
				doIt = jniEnv->CallBooleanMethod(field, methodId, CreateJNIString(jniEnv, url), isMain);

			} else {

//TODO: how to test this?
#if defined(WIN32)
				HANDLE handle = CreateEvent(0, TRUE, FALSE, 0);
				CefRefPtr<OnBeforeResourceLoadTask> task(new OnBeforeResourceLoadTask(field, methodId, CreateJNIString(jniEnv, url), isMain, handle));
				CefPostTask(TID_UI, static_cast<CefRefPtr<CefTask> >(task));
				WaitForSingleObject(handle, INFINITE);
				CloseHandle(handle);
				doIt = task->doIt;
#elif defined(linux)
				sem_t handle;
				CefRefPtr<OnBeforeResourceLoadTask> task(new OnBeforeResourceLoadTask(field, methodId, CreateJNIString(jniEnv, url), isMain, handle));
				CefPostTask(TID_UI, static_cast<CefRefPtr<CefTask> >(task));
				sem_wait(&handle);
				doIt = task->doIt;
#endif
			}

			if (doIt == JNI_FALSE) {
				return false; // Cancel loading			
			}
		} else {
			jniEnv->ExceptionClear();
		}
	
	} else {
		jniEnv->ExceptionClear();
	}

	return true; // allow loading
}

void ClientRequestHandler::NotifyURLChanged(CefString url, bool isMain) {
	JNIEnv* jniEnv = GetJNIEnv();
	Log(jniEnv, "ClientRequestHandler::NotifyURLChanged");

	jobject field = GetJavaField(jniEnv, m_jBrowser, "N_LocationListener", "Lcom/ti/chromium/browser/internal/cef/N_LocationListener;");
	if (field != NULL) {
		jclass clazz = jniEnv->GetObjectClass(field);
		jmethodID methodId = jniEnv->GetMethodID(clazz, "changed", "(Ljava/lang/String;Z)V");

		if (methodId != 0) {
			jniEnv->CallVoidMethod(field, methodId, CreateJNIString(jniEnv, url), isMain);

		} else {
			jniEnv->ExceptionClear();
		}
	
	} else {
		jniEnv->ExceptionClear();
	}
}