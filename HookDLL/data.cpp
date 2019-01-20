#pragma once
#include "stdafx.h"
#include "data.h"
#include "global.h"
#include "picojson.h"
#include <shlobj.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <string>

namespace WebShellKillHook {

	Data::Data() {
		data.set<>(picojson::array());
	}

	std::string Data::serialize() {
		return data.serialize();
	}

	LRESULT CALLBACK Data::hookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
		CWPSTRUCT msg = *(CWPSTRUCT*)lParam;
		if (msg.message == WM_ENABLE) {
			if (isScanning) {
				isScanning = false;
				Global::event.emit(Global::EVENT_SCAN_END);
			}
		}
		else if (msg.message == LVM_INSERTITEMA) {
			currentColumnIndex = -1;
			isWaitingForText = true;
			isSecondCall = 0;
			item.set<>(picojson::object());
			isScanning = true;
		}
		else if (msg.message == LVM_SETITEMA) {
			if (isWaitingForText) {
				data.get<picojson::array>().push_back(item);
				item.get<picojson::object>().clear();
				isWaitingForText = false;
			}
		}
		else if (msg.message == LVM_SETITEMTEXTA) {
			if (isWaitingForText && ++isSecondCall % 2 == 1) {
				currentColumnIndex++;
				LPSTR str = (LPSTR)(malloc(sizeof(LPSTR) * 255));
				ListView_GetItemText(msg.hwnd, ((LVITEMA*)lParam)->iItem, currentColumnIndex, str, 255);
				item.get<picojson::object>()[columnMap[currentColumnIndex]] = picojson::value(str);
				free(str);
			}
		}
		return CallNextHookEx(0, nCode, wParam, lParam);
	}
}