#include <stdio.h>

#include "MemoryExt.h"
#include "Signatures.h"

 
int main()
{
	int iGamePID = 0;
	printf("Enter the process id for Stronghold 2: ");
	scanf_s("%d", &iGamePID); // Grab the ProcID of Stronghold2 from the user's input.

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, iGamePID);
	if (hProc)
	{	
		MemoryExt pMemoryExt(hProc); // Init the MemoryExt class with the handle to the process I want to modify.

		printf("\nSearching for AI MP Pattern!\n");
		int iAddrToNOP = pMemoryExt.FindPatternMainModule(szMPAIFuncSig, szMPAIFuncMask); // Try to find the pattern for the AI Disable instructions.

		if (iAddrToNOP != -1) // Check to make sure an invalid address has not been returned.
		{
			printf("Found pattern at: 0x%x\n", iAddrToNOP);

			if (pMemoryExt.NOPBytes((LPVOID)iAddrToNOP, 12)) // Try to NOP patch the AI disable bytes so they are no longer executed.
				printf("Patched MP AI Disable Instructions\n\n");
			else
				printf("Failed to patch MP AI Disable Instructions!\n\n");
		}
		else
		{
			printf("Failed to find pattern!\n\n");
		}
	}
	else
	{
		printf("Unable to obtain handle to the Stronghold 2 process! Try launching as administrator if you havn't already.\n");
	}

	system("pause"); // Suspend the console app at the end so the user can see the results.
}