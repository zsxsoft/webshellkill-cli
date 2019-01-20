#pragma once
#pragma once
#include "stdafx.h"
#include "picojson.h"
#include <shlobj.h>
#include <Windows.h>
#include <tlhelp32.h>
class WebShellKillData {

private:
	int currentColumnIndex = -1;
	int isSecondCall = 0;
	bool isWaitingForText = false;
	bool isScanning = false;
	std::string columnMap[6] = { "file", "level", "description", "size", "time", "crc32" };
	picojson::value data;
	picojson::value item;

public:
	HWND indexButtonScanHwnd = 0;
	HWND startButtonScanHwnd = 0;
	void initialize();
	LRESULT CALLBACK hookCallback(int nCode, WPARAM wParam, LPARAM lParam);
	void something(UINT message, WPARAM wParam, LPARAM lParam);
};