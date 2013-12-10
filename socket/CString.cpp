#pragma once

#include "CString.h"


void CString::Set(AMX* amx, cell param, char* str)
{
	cell *destination;
	amx_GetAddr(amx, param, &destination);
	amx_SetString(destination, str, 0, 0, strlen(str)+1);
	return;
}

int CString::Get(AMX* amx, cell param, char*& dest)
{
	cell *ptr;
	int len;
	amx_GetAddr(amx, param, &ptr);
	amx_StrLen(ptr, &len);
	dest = (char*)malloc((len * sizeof(char))+1);
	amx_GetString(dest, ptr, 0, UNLIMITED);
	dest[len] = '\0';
	return len;
}