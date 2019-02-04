#pragma once
#include "stdafx.h"
#include <Windows.h>
#pragma comment(linker, "/EXPORT:GetFileVersionInfoA=_AheadLib_GetFileVersionInfoA,@1")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoByHandle=_AheadLib_GetFileVersionInfoByHandle,@2")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExA=_AheadLib_GetFileVersionInfoExA,@3")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExW=_AheadLib_GetFileVersionInfoExW,@4")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeA=_AheadLib_GetFileVersionInfoSizeA,@5")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExA=_AheadLib_GetFileVersionInfoSizeExA,@6")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExW=_AheadLib_GetFileVersionInfoSizeExW,@7")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeW=_AheadLib_GetFileVersionInfoSizeW,@8")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoW=_AheadLib_GetFileVersionInfoW,@9")
#pragma comment(linker, "/EXPORT:VerFindFileA=_AheadLib_VerFindFileA,@10")
#pragma comment(linker, "/EXPORT:VerFindFileW=_AheadLib_VerFindFileW,@11")
#pragma comment(linker, "/EXPORT:VerInstallFileA=_AheadLib_VerInstallFileA,@12")
#pragma comment(linker, "/EXPORT:VerInstallFileW=_AheadLib_VerInstallFileW,@13")
#pragma comment(linker, "/EXPORT:VerLanguageNameA=_AheadLib_VerLanguageNameA,@14")
#pragma comment(linker, "/EXPORT:VerLanguageNameW=_AheadLib_VerLanguageNameW,@15")
#pragma comment(linker, "/EXPORT:VerQueryValueA=_AheadLib_VerQueryValueA,@16")
#pragma comment(linker, "/EXPORT:VerQueryValueW=_AheadLib_VerQueryValueW,@17")

#define EXTERNC extern "C"
#define NAKED __declspec(naked)
#define EXPORT __declspec(dllexport)

#define ALCPP EXPORT NAKED
#define ALSTD EXTERNC EXPORT NAKED void __stdcall
#define ALCFAST EXTERNC EXPORT NAKED void __fastcall
#define ALCDECL EXTERNC NAKED void __cdecl

namespace AheadLib
{
	HMODULE m_hModule = NULL;
	DWORD m_dwReturn[17] = { 0 };

	inline BOOL WINAPI Load()
	{
		TCHAR tzPath[MAX_PATH];
		TCHAR tzTemp[MAX_PATH * 2];

		GetSystemDirectory(tzPath, MAX_PATH);
		lstrcat(tzPath, TEXT("\\version"));
		m_hModule = LoadLibrary(tzPath);
		if (m_hModule == NULL)
		{
			wsprintf(tzTemp, TEXT("No Original Version.dll"), tzPath);
			MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
		}

		return (m_hModule != NULL);
	}

	inline VOID WINAPI Free()
	{
		if (m_hModule)
		{
			FreeLibrary(m_hModule);
		}
	}

	FARPROC WINAPI GetAddress(PCSTR pszProcName)
	{
		FARPROC fpAddress;
		CHAR szProcName[16];
		TCHAR tzTemp[MAX_PATH];

		if (m_hModule == NULL)
		{
			if (Load() == FALSE)
			{
				ExitProcess(-1);
			}
		}

		fpAddress = GetProcAddress(m_hModule, pszProcName);
		if (fpAddress == NULL)
		{
			if (HIWORD(pszProcName) == 0)
			{
				wsprintf(szProcName, "%d", pszProcName);
				pszProcName = szProcName;
			}

			wsprintf(tzTemp, TEXT("No Such Function %s"), pszProcName);
			MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
			ExitProcess(-2);
		}

		return fpAddress;
	}
}
using namespace AheadLib;
void Hijack();

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		Hijack();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		Free();
	}

	return TRUE;
}


ALCDECL AheadLib_GetFileVersionInfoA(void)
{
	GetAddress("GetFileVersionInfoA");
	__asm JMP EAX;
}

ALCDECL AheadLib_GetFileVersionInfoByHandle(void)
{
	GetAddress("GetFileVersionInfoByHandle");
	__asm JMP EAX;
}


ALCDECL AheadLib_GetFileVersionInfoExA(void)
{
	GetAddress("GetFileVersionInfoExA");
	__asm JMP EAX;
}

ALCDECL AheadLib_GetFileVersionInfoExW(void)
{
	GetAddress("GetFileVersionInfoExW");
	__asm JMP EAX;
}

ALCDECL AheadLib_GetFileVersionInfoSizeA(void)
{
	GetAddress("GetFileVersionInfoSizeA");
	__asm JMP EAX;
}

ALCDECL AheadLib_GetFileVersionInfoSizeExA(void)
{
	GetAddress("GetFileVersionInfoSizeExA");
	__asm JMP EAX;
}

ALCDECL AheadLib_GetFileVersionInfoSizeExW(void)
{
	GetAddress("GetFileVersionInfoSizeExW");
	__asm JMP EAX;
}

ALCDECL AheadLib_GetFileVersionInfoSizeW(void)
{
	GetAddress("GetFileVersionInfoSizeW");
	__asm JMP EAX;
}

ALCDECL AheadLib_GetFileVersionInfoW(void)
{
	GetAddress("GetFileVersionInfoW");
	__asm JMP EAX;
}

ALCDECL AheadLib_VerFindFileA(void)
{
	GetAddress("VerFindFileA");
	__asm JMP EAX;
}

ALCDECL AheadLib_VerFindFileW(void)
{
	GetAddress("VerFindFileW");
	__asm JMP EAX;
}

ALCDECL AheadLib_VerInstallFileA(void)
{
	GetAddress("VerInstallFileA");
	__asm JMP EAX;
}

ALCDECL AheadLib_VerInstallFileW(void)
{
	GetAddress("VerInstallFileW");
	__asm JMP EAX;
}

ALCDECL AheadLib_VerLanguageNameA(void)
{
	GetAddress("VerLanguageNameA");
	__asm JMP EAX;
}

ALCDECL AheadLib_VerLanguageNameW(void)
{
	GetAddress("VerLanguageNameW");
	__asm JMP EAX;
}

ALCDECL AheadLib_VerQueryValueA(void)
{
	GetAddress("VerQueryValueA");
	__asm JMP EAX;
}

ALCDECL AheadLib_VerQueryValueW(void)
{
	GetAddress("VerQueryValueW");
	__asm JMP EAX;
}
