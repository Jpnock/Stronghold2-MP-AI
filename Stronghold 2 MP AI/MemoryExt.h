#pragma once
#include <Windows.h>
#include <Dbghelp.h>
#include <TlHelp32.h>

class MemoryExt
{
public:
	MemoryExt(HANDLE hProc); // Init with handle to process that you want to use the class on.

	int FindPattern(char* bMask, char *szMask); // Very, very, very slow due to mass RPM calls.
	int FindPatternMainModule(char* bMask, char *szMask);
	BOOL NOPBytes(LPVOID lpBaseAddress, unsigned int iNumOfBytes);

private:
	HANDLE hProc;
};

