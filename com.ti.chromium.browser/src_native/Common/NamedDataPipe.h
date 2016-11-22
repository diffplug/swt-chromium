#ifndef NAMEDDATAPIPE_H_
#define NAMEDDATAPIPE_H_

#include <string>

#if defined(WIN32)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#elif defined(linux)
#include <fcntl.h>
#include <sys/stat.h>
#endif

class NamedDataPipe {
public:
	NamedDataPipe(std::string pipeName, bool create);
	virtual ~NamedDataPipe();

	void setResult(std::string result);
	std::string getResult();
private:
#if defined(WIN32)
	HANDLE m_handle;
	HANDLE m_threadHandle;
#elif defined(linux)
	int m_handle;
#endif

	bool m_owner;

	void waitForMonitorSignal();
public:
	std::string m_pipeName;
	int m_threadTimeout;
	bool m_pipeCompleted;
};

#endif /* DATAPIPE_H_ */
