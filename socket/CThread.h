#pragma once

#include "main.h"

#ifdef WIN32
typedef DWORD (*ThreadFunc)(void* lpParam);
#else
typedef void* (*ThreadFunc)(void* lpParam);
#endif

class CThread 
{
public:
#ifdef WIN32
	HANDLE Start(ThreadFunc Function, void* param);
	int Kill(HANDLE thread);
#else
	pthread_t Start(ThreadFunc Function, void* param);
	int Kill(pthread_t thread);
#endif
};