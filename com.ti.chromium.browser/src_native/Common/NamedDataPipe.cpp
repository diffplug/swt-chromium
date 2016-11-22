#include "NamedDataPipe.h"

#define CEF_EOF 	"EOF"
#define CEF_EOFSIZE 3
#define BUFFER_SIZE 64

// must be greater than 100
#define TIMEOUT 500

DWORD ThreadProc(LPVOID lpdwThreadParam);

NamedDataPipe::NamedDataPipe(std::string pipeName, bool owner) {
	m_pipeName = pipeName;
	m_owner = owner;
	m_pipeCompleted = false;

#if defined(WIN32)
	m_threadHandle = NULL;

	if (owner) {
		m_handle = CreateNamedPipe(
			pipeName.c_str(),
			PIPE_ACCESS_INBOUND, 
			PIPE_TYPE_BYTE, 
			1,  
			BUFFER_SIZE,
			BUFFER_SIZE, 
			0,
			NULL);

	} else {
		m_handle = CreateFile(
			pipeName.c_str(), 
			PIPE_ACCESS_OUTBOUND,
			0, 
			NULL, 
			OPEN_EXISTING,
			0, 
			NULL);
	}
	
#elif defined(linux)
	if (owner) {
		mkfifo(pipeName.c_str(), 0666);
	}
	m_handle = open(pipeName.c_str(), O_RDWR);
#endif
}

NamedDataPipe::~NamedDataPipe() {

#if defined(WIN32)
	try {
		if (!m_owner) {
			DWORD bytesWritten = 0;
			WriteFile(m_handle, CEF_EOF, CEF_EOFSIZE, &bytesWritten, NULL);
		} else {
			DisconnectNamedPipe(m_handle);
		}
		CloseHandle(m_handle);
	} catch (...) {}
#elif defined(linux)
	if (!m_owner)
		write(m_handle, CEF_EOF, CEF_EOFSIZE);
	close(m_handle);
#endif
}

void NamedDataPipe::waitForMonitorSignal() {
	if (m_threadHandle != NULL) {

#if defined(WIN32)
		::WaitForSingleObject(m_threadHandle, TIMEOUT);
#endif

		m_threadHandle = NULL;		
	}
}

void NamedDataPipe::setResult(std::string result) {
#if defined(WIN32)
	DWORD bytesWritten = 0;
	WriteFile(m_handle, result.c_str(), result.size(), &bytesWritten, NULL);
#elif defined(linux)
	write(m_handle, result.c_str(), result.size());
#endif
}

std::string NamedDataPipe::getResult() {
	if (!m_owner)
		return "Result is available for pipe owner only.";

	std::string result;
	char buf[BUFFER_SIZE+CEF_EOFSIZE];
#if defined(WIN32)
	
	/* create a thread to monitor broken pipe */
	m_threadTimeout = TIMEOUT - 100;
	m_pipeCompleted = false;
	m_threadHandle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ThreadProc, this, NULL, NULL);
	if (m_threadHandle == NULL) {
		return result;
	}

	m_pipeCompleted = false;
	ConnectNamedPipe(m_handle, NULL);
	DWORD bytesRead = 0;
	while (ReadFile(m_handle, buf, BUFFER_SIZE+CEF_EOFSIZE, &bytesRead, NULL) > 0) {
#elif defined(linux)
	int bytesRead = 0;
	while ((bytesRead = read(m_handle, buf, BUFFER_SIZE)) > 0) {
#endif
		std::string tmp = std::string(buf, bytesRead);
		bool done = false;
		if (tmp.find(CEF_EOF) == (bytesRead - CEF_EOFSIZE)) {
			tmp = tmp.substr(0, bytesRead-CEF_EOFSIZE);
			done = true;
		}
		result += tmp;

		if (done)
			break;
	}

#if defined(WIN32)
	m_pipeCompleted = true;
	waitForMonitorSignal();
#endif

	return result;
}

DWORD ThreadProc(LPVOID lpdwThreadParam) {
	NamedDataPipe* pipe = (NamedDataPipe*)lpdwThreadParam;
	while ((pipe->m_threadTimeout > 0) && !pipe->m_pipeCompleted) {
		Sleep(10);
		pipe->m_threadTimeout -= 10;
	}

	// flush the pipe
	if (!pipe->m_pipeCompleted) {
		try {
			NamedDataPipe cleanupPipe(pipe->m_pipeName, false);
			cleanupPipe.setResult("flush");
		} catch (...) {}
	}

	return 0;
}