// FindDMAAddy.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <vector>

// Type Defintion (no memory address). the actually memory space for objects of this class will only be created when you allocate memory for an object.
class PlayerClass {
	int health;
	int armour;
	char* name;
};

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	
	for (unsigned int i = 0; i < offsets.size(); ++i) {
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

void ErrorExit() {
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	if (FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL) == 0)
	{
		MessageBox(NULL, TEXT("FormatMessage failed"), TEXT("Error"), MB_OK);
		ExitProcess(dw);
	}

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	ExitProcess(dw);
}

int main() {
	PlayerClass* localPlayer = nullptr; // Stack: localPlayer variable.
	localPlayer = new PlayerClass; // Heap: new object created, localPlauer holds the address of that object.

	// Write a line of code that dereference [ac_client.exe+195404] to get 0x0084C7F0 - localPlayer pointer value
	uintptr_t dynamicPtrBaseAddr = 0x6D4008;
	DWORD processID = 22196; // Double Word = DWORD = unsigned (non-negative) 32-bit integer.

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION,
								  FALSE, processID);

	if (!hProcess) { // Checks if hProcess is NULL 
		std::cerr << "Failed to open process..." << std::endl;
		return 1; // return 1 to indicate that an error occured or the program didn't complete as expected.
	}
	else {
		std::cout << hProcess << std::endl;
	}

	//uintptr_t healthAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, { 0xEC });

	uintptr_t healthAddress = 0x6D40F4;
	uintptr_t healthValue = 0;
	int newHealth = 1337;

	ReadProcessMemory(hProcess, (BYTE*)healthAddress, &healthValue, sizeof(healthAddress), NULL);
	BOOL success = WriteProcessMemory(hProcess, (LPVOID)healthAddress, &newHealth, sizeof(newHealth), NULL);
	if (success) {
		std::cout << "Successfully wrote " << healthValue << " to address" << std::hex << healthAddress << std::endl;
	}
	else {
		ErrorExit();
		std::cerr << "Failed to write memory." << std::endl;
	}

	std::cout << "Decimal Value for Health: " << healthValue << std::endl;

	CloseHandle(hProcess);

}


