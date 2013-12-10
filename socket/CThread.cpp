#pragma once

#include "CThread.h"

#ifdef WIN32
HANDLE CThread::Start(ThreadFunc Function, void* param)
#else
pthread_t CThread::Start(ThreadFunc Function, void* param)
#endif
{
#ifdef WIN32
	DWORD dwThreadId = 0;
	HANDLE threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Function, param, 0, &dwThreadId);
	CloseHandle(threadHandle);
#else
	pthread_t threadHandle;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	int error = pthread_create(&threadHandle, &attr, Function, param);
#endif
	return threadHandle;
}

#ifdef WIN32
int CThread::Kill(HANDLE thread)
#else
int CThread::Kill(pthread_t thread)
#endif
{
#ifdef WIN32
	if(WaitForSingleObject(thread, NULL) == WAIT_TIMEOUT) {
		TerminateThread(thread, NULL);
	}
#else
	pthread_join(thread, NULL);
#endif
	return 1;
}