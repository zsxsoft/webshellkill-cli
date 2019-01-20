#pragma once
#include "stdafx.h"
#include "data.h"
#include "global.h"
#include "picojson.h"
#include <shlobj.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>

namespace WebShellKillHook {

	Data::Data() {
		data.set<>(picojson::array());
		WebShellKillHook::Global::event.on(WebShellKillHook::Global::EVENT_SCAN_START, [&]() {
			isScanning = true;
			enableCount = 0;
			realFoundSomething = false;
		});
	}

	std::string Data::serialize() {
		return data.serialize();
	}

	LRESULT CALLBACK Data::hookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
		CWPSTRUCT msg = *(CWPSTRUCT*)lParam;
		if (msg.message == WM_ENABLE) {
			if (realFoundSomething) {
				realFoundSomething = false;
				isScanning = false;
				Global::event.emit(Global::EVENT_SCAN_END);
			}
			else if (isScanning) {
				if (++enableCount == 7) {
					isScanning = false;
					Global::event.emit(Global::EVENT_SCAN_END);
				}
			}
		}
		else if (msg.message == LVM_INSERTITEMA) {
			currentColumnIndex = -1;
			isWaitingForText = true;
			isSecondCall = 0;
			item.set<>(picojson::object());
			realFoundSomething = true;
		}
		else if (msg.message == LVM_SETITEMA) {
			if (isWaitingForText) {
				data.get<picojson::array>().push_back(item);
				std::cerr << item.serialize() << std::endl;
				item.get<picojson::object>().clear();
				isWaitingForText = false;
			}
		}
		else if (msg.message == LVM_SETITEMTEXTA) {
			++isSecondCall;
			if (isWaitingForText && (isSecondCall % 2 == 0)) {
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