// HookDLL.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "WebShellKillData.h"
#include "WebShellKillConfig.h"
#include <string>
#include <iostream>
#include <shlobj.h>
#include <Windows.h>
#include <tlhelp32.h>
#include "detours.h"
#include <shellapi.h>
#include "CLI11.hpp"
#pragma comment (lib, "detours.lib")

DWORD threadId;
WebShellKillData data;
std::string scanPath;

LPITEMIDLIST ParsePidlFromPath(LPCSTR path) {
	OLECHAR szOleChar[MAX_PATH];
	LPSHELLFOLDER lpsfDeskTop;
	LPITEMIDLIST lpifq;
	ULONG ulEaten, ulAttribs;
	HRESULT hres;
	SHGetDesktopFolder(&lpsfDeskTop);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, path, -1, szOleChar, sizeof(szOleChar));
	hres = lpsfDeskTop->ParseDisplayName(NULL, NULL, szOleChar, &ulEaten, &lpifq, &ulAttribs);
	hres = lpsfDeskTop->Release();
	if (FAILED(hres))
		return NULL;
	return lpifq;
}

LRESULT CALLBACK hookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	//printf("%d\n", nCode);
	return data.hookCallback(nCode, wParam, lParam);
}

static LPITEMIDLIST(WINAPI *oldSHBrowseForFolderA)(LPBROWSEINFOA lpbi) = SHBrowseForFolderA;
LPITEMIDLIST WINAPI hookSHBrowseForFolderA(LPBROWSEINFOA lpbi)
{
	LPITEMIDLIST pList = ParsePidlFromPath(scanPath.c_str());
	return pList;
}

bool done = false;
static LSTATUS(WINAPI *oldRegQueryValueExA)(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) = RegQueryValueExA;
LSTATUS WINAPI hookRegQueryValueExA(HKEY hKey, LPSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE  lpData, LPDWORD lpcbData) {
	printf("%s\n", lpValueName);
	if (strcmp(lpValueName, "ALL_Check_dir") == 0) {
		done = true;
	}

	return oldRegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

static HWND(WINAPI *oldCreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = CreateWindowExA;
HWND WINAPI hookCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
	HWND ret = oldCreateWindowExA(hWndParent == 0 ? WS_EX_TOOLWINDOW : dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if (hWndParent == 0) {
		SetWindowLong(ret, GWL_STYLE, 0);
	}
	if (lpWindowName != nullptr) {
		// printf("%s\n", lpWindowName);
		if (strcmp(lpWindowName, "自定义扫描") == 0) {
			data.indexButtonScanHwnd = ret;
		}
		else if (strcmp(lpWindowName, "全部清理") == 0) {
		}
	}
	return ret;
}
void startScan() {
	PostMessage(data.indexButtonScanHwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
	PostMessage(data.indexButtonScanHwnd, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
}

void setScanPath() {
	scanPath = WebShellKillConfig::scanList[0];
	// Wine should hook here
	auto ret = SetWindowsHookExA(WH_CALLWNDPROC, hookCallback, 0, GetCurrentThreadId());
	if (ret == nullptr) {
		printf("Error! %x\n", GetLastError());
		ExitProcess(GetLastError());
	}
	else {
		printf("Successfully hooked!");
	}
	startScan();
}


static BOOL(WINAPI *oldShowWindow)(HWND hWnd, int  nCmdShow) = ShowWindow;
BOOL WINAPI hookShowWindow(HWND hWnd, int nCmdShow) {
	//printf("Show Window");
	if (done) {
		setScanPath();
		done = false;
	}
	return oldShowWindow(hWnd, 0);
}




static LPTSTR(WINAPI *oldGetCommandLineA)() = GetCommandLineA;
// v2.0.9, 001329FB. Don't let it get argv
LPTSTR WINAPI hookGetCommandLineA()
{
	LPTSTR ret = new char[MAX_PATH];
	LPWSTR *szArglist;
	int nArgs;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	sprintf_s(ret, lstrlenW(szArglist[0]) + 1, "%ls", szArglist[0]);
	return ret;
}


void Hijack() {
	threadId = GetCurrentThreadId();
	data.initialize();
	if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
		AllocConsole();
	}
	freopen("CONOUT$", "w", stdout);
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((void **)&oldSHBrowseForFolderA, hookSHBrowseForFolderA);
	DetourAttach((void **)&oldRegQueryValueExA, hookRegQueryValueExA);
	DetourAttach((void **)&oldGetCommandLineA, hookGetCommandLineA);
	DetourAttach((void **)&oldCreateWindowExA, hookCreateWindowExA);
	DetourAttach((void **)&oldShowWindow, hookShowWindow);
	DetourTransactionCommit();

	printf("Thread ID = %d\n", GetCurrentThreadId());
	WebShellKillConfig::initialize();

	return;
}
