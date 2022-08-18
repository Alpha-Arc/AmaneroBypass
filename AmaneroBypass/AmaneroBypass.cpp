#include <windows.h>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <regex>
#include <TlHelp32.h>
unsigned char target[6] = { 0x74, 0x19, 0x8B, 0x85, 0x78, 0xFF };
unsigned char repack[6] = { 0xEB, 0x19, 0x8B, 0x85, 0x78, 0xFF };

INT64 getAddress(HANDLE process, unsigned char target[6])
{
	unsigned char* read = new unsigned char[6];
	for (INT64 addr = 0x00400000; addr <= 0x10000000; ++addr, ReadProcessMemory(process, (LPVOID)addr, read, 6, 0))
		if (memcmp(read, target, 6) == 0)
			return addr;
	return false;
}

void write(HANDLE process, INT64 addr, unsigned char repack[6]) { WriteProcessMemory(process, (LPVOID)addr, repack, 6, 0); }

std::string wcToString(WCHAR wc[260])
{
	char temp[260];
	char defChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, wc, -1, temp, 260, &defChar, NULL);
	std::string result(temp);
	return result;
}

DWORD getPid(std::string target)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return false;

	bool bResult = Process32First(hProcessSnap, &pe32);

	while (bResult)
	{
		if (wcToString(pe32.szExeFile) == target)
			return pe32.th32ProcessID;
		bResult = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	return false;
}

int main()
{
	DWORD pid = getPid("ConfigTool.exe");
	if (!pid)
	{
		printf("Please Open The \"ConfigTool.exe\"\nAnd Restart This Program\n");
		return 0;
	}
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	write(process, getAddress(process, target), repack);

	CloseHandle(process);
	printf("pathSucceed\n");
	system("pause");
	return 0;
}