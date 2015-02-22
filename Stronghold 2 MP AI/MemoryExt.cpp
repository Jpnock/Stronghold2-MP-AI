#include "MemoryExt.h"

MemoryExt::MemoryExt(HANDLE hProc)
{
	this->hProc = hProc;
}

// Very, very, very slow due to mass RPM calls.
int MemoryExt::FindPattern(char* bMask, char *szMask)
{
	DWORD dwPos = 0;
	DWORD dwSearchLen = strlen(szMask) - 1;
	BYTE *buffer = new BYTE[1];

	for (DWORD dwCurAddress = 0x401000; dwCurAddress < 0x7FFFFFF; dwCurAddress++) // Scan from base address to max 32bit addr.
	{
		if (szMask[dwPos] == '?') // Pass if byte of sig is unknown.
		{
			dwPos++;
		}
		else if (szMask[dwPos] == '\0') // If we are at the end of sig then return found addr.
		{
			delete[] buffer;
			return (dwCurAddress - 1) - dwSearchLen;
		}
		else 
		{
			ReadProcessMemory(this->hProc, (LPCVOID)dwCurAddress, buffer, 1, 0);
			if (*buffer == (BYTE)bMask[dwPos]) // Compare read memory to the sig we are looking for.
				dwPos++;
			else
				dwPos = 0;
		}
	}

	delete[] buffer;
	return -1; // If we havn't returned beforehand, we found no sig so return an invalid addr.
}

int MemoryExt::FindPatternMainModule(char* bMask, char *szMask)
{
	DWORD dwPos = 0;
	DWORD dwMaskLen = strlen(szMask) - 1;
	DWORD dwMainModStart = 0;
	DWORD dwMainModLen = 0;

	MODULEENTRY32 lpme;
	lpme.dwSize = sizeof(MODULEENTRY32);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(this->hProc));
	Module32First(hSnap, &lpme);
	dwMainModStart = (DWORD)lpme.modBaseAddr;
	dwMainModLen = lpme.modBaseSize - dwMainModStart;

	BYTE *buffer = new byte[dwMainModLen];
	BOOL bRPM = ReadProcessMemory(hProc, (LPCVOID)dwMainModStart, buffer, dwMainModLen, 0);;

	for (DWORD dwCurAddress = 0; dwCurAddress < dwMainModLen; dwCurAddress++)
	{
		if (szMask[dwPos] == '?')
		{
			dwPos++;
		}
		else if (szMask[dwPos] == '\0')
		{
			delete[] buffer;
			return dwMainModStart + (dwCurAddress - 1) - dwMaskLen;
		}
		else
		{
			if (buffer[dwCurAddress] == (BYTE)bMask[dwPos])
				dwPos++;
			else
				dwPos = 0;
		}
	}

	delete[] buffer;
	return -1;
}

BOOL MemoryExt::NOPBytes(LPVOID lpBaseAddress, unsigned int iNumOfBytes)
{
	byte *buffer = new byte[iNumOfBytes]; memset(buffer, 0x90, iNumOfBytes); // Create a byte array and fill it with 0x90 (NOP instr. code).
	BOOL bWPM = WriteProcessMemory(this->hProc, lpBaseAddress, buffer, iNumOfBytes, NULL);

	delete[] buffer;
	return bWPM;
}