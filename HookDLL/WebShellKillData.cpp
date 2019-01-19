#pragma once
#include "stdafx.h"
#include "WebShellKillData.h"
#include "picojson.h"
#include <shlobj.h>
#include <Windows.h>
#include <tlhelp32.h>

void WebShellKillData::initialize() {
	data.set<>(picojson::array());
}

LRESULT CALLBACK WebShellKillData::hookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	CWPSTRUCT msg = *(CWPSTRUCT*)lParam;
	if (msg.message == LVM_INSERTITEMA) {
		currentColumnIndex = -1;
		isWaitingForText = true;
		isSecondCall = 0;
		item.set<>(picojson::object());
	}
	else if (msg.message == LVM_SETITEMA) {
		if (isWaitingForText) {
			data.get<picojson::array>().push_back(item);
			item.get<picojson::object>().clear();
			isWaitingForText = false;
			std::cout << data.serialize() << std::endl << std::endl;
		}
	}
	else if (msg.message == LVM_SETITEMTEXTA) {
		if (isWaitingForText && ++isSecondCall % 2 == 0) {
			currentColumnIndex++;
			LPSTR str = (LPSTR)(malloc(sizeof(LPSTR) * 255));
			ListView_GetItemText(msg.hwnd, ((LVITEMA*)lParam)->iItem, currentColumnIndex, str, 255);
			item.get<picojson::object>()[columnMap[currentColumnIndex]] = picojson::value(str);
			free(str);
		}
	}
	return CallNextHookEx(0, nCode, wParam, lParam);
}
