#pragma once
#include "stdafx.h"
#include "EventEmitter.h"
#include <string>
#include <vector>

namespace WebShellKillHook {
	namespace Global {
		const int EVENT_READY = 1;
		const int EVENT_GET_BUTTON_HWND = 2;
		const int EVENT_SCAN_END = 3;
		const int EVENT_DONE = 5;
		extern EventEmitter event;
		extern std::vector<std::string>::iterator currentIterator;

		void initialize();
		void startScanQueue();
		void startScan();
	}
}