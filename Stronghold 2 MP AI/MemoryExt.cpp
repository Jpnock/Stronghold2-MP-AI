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

BOOL MemoryExt::NOPBytes(LPVOID lpBaseAddress, unsigned int iNumOfBytes)
{
	byte *buffer = new byte[iNumOfBytes]; memset(buffer, 0x90, iNumOfBytes); // Create a byte array and fill it with 0x90 (NOP instr. code).
	BOOL bWPM = WriteProcessMemory(this->hProc, lpBaseAddress, buffer, iNumOfBytes, NULL);

	delete[] buffer;
	return bWPM;
}