#pragma once

// taken from my sql plugin

#include "main.h"

class CString
{
public:
	void Set( AMX* amx, cell param, char* str);
	int Get(AMX* amx, cell param, char*& dest);
};