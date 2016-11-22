#include <stdlib.h>

#if defined(WIN32)
#include <tchar.h>
#endif

#include "ClientApp.h"
#include "ClientRenderProcessHandler.h"
#include <include/cef_app.h>

#if defined(WIN32)
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	CefMainArgs main_args(hInstance);
#elif defined(linux)
int main(int argc, char *argv[]) {
	CefMainArgs main_args(argc, argv);
#endif

#if defined(WIN32)
//	MessageBox(NULL, "CEFSubProcess", "Halt at main", 0);
#elif defined(linux)
/*
	gtk_init(&argc, &argv);
	GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_INFO,
                                  GTK_BUTTONS_OK,
                                  "CEFSubProcess");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
*/
#endif

	CefRefPtr<ClientApp> app(new ClientApp(new ClientRenderProcessHandler));

	void* sandbox_info = NULL;

#if CEF_ENABLE_SANDBOX
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
	if (exit_code >= 0) {
		return exit_code;
	}

	return 0;
}
