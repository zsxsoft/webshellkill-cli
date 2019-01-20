#pragma once
#include "stdafx.h"
#include "config.h"
#include "global.h"
#include "data.h"
#include <string>
#include <iostream>
#include <vector>

namespace WebShellKillHook {
	namespace Global {
		std::vector<std::string>::iterator currentIterator;

		EventEmitter event;
		Data data;
		void initialize() {
			event.on(EVENT_READY, [] {
				auto ret = SetWindowsHookExA(WH_CALLWNDPROC, [](int nCode, WPARAM wParam, LPARAM lParam) {
					return data.hookCallback(nCode, wParam, lParam);
				}, 0, GetCurrentThreadId());
				if (ret == nullptr) {
					printf("Error! %x\n", GetLastError());
					ExitProcess(GetLastError());
				}
				startScanQueue();
			});
			event.on<HWND>(EVENT_GET_BUTTON_HWND, [](HWND buttonHwnd) {
				data.indexButtonScanHwnd = buttonHwnd;
			});
			event.on(EVENT_SCAN_END, []() {
				currentIterator++;
				if (currentIterator == Config::scanList.end()) {
					event.emit(EVENT_DONE);
					return;
				}
				startScan();
			});
			event.on(EVENT_DONE, []() {
				std::cout << data.serialize() << std::endl;
				ExitProcess(0);
			});
		};

		void startScanQueue() {
			currentIterator = Config::scanList.begin();
			startScan();
		}

		void startScan() {
			PostMessage(data.indexButtonScanHwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
			PostMessage(data.indexButtonScanHwnd, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
		}
	}
}