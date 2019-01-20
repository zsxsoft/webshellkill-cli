#pragma once
#include "stdafx.h"
#include "config.h"
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <filesystem>

using std::cout;
using std::endl; 

std::map<string, string> Config::argvFlagMap;
CLI::App* Config::app;
std::vector<std::string> Config::scanList;

std::string Config::outputPath = "";

void Config::insert(string registry, string option, string defaultValue, string helpString) {
	argvFlagMap[registry] = defaultValue;
	app->add_option(option, argvFlagMap[registry], helpString);
}

void Config::initialize() {
	app = new CLI::App{ "WebShellKill CLI" };
	LPWSTR *szArglist;
	int nArgs;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	
	insert("Check_Hide_File", "-i,--show-hidden-file", "1", "扫描隐藏文件");
	insert("Show_Zend_File", "-z,--show-zend-file", "1", "显示经过Zend加密的文件");
	insert("Auto_Check_New_Ver", "-a,--auto-check-new-version", "1", "自动更新行为库");
	insert("Hide_Levl1", "-d,--hide-level1", "1", "隐藏低级别威胁");
	insert("Check_Type", "-t,--type", "1", "TBD");
	insert("Show_Hide_dir_", "-e,--exclude-dir", "0", "排除指定的文件夹（TBD）");

	app->add_option("-f,--output", outputPath, "输出到文件");

	app->allow_extras(true);

	const char** args = new const char*[nArgs];
	for (auto i = 0; i < nArgs; i++) {
		size_t len = lstrlenW(szArglist[i]);
		char* arg = new char[len * 2];
		sprintf_s(arg, len + 1, "%ls", szArglist[i]);
		args[i] = arg;
	}

	try {
		app->parse(nArgs, args);
		scanList = app->remaining();
		if (scanList.size() == 0) {
			throw CLI::CallForHelp();
		}
		for (auto &f : scanList) {
			if (!std::experimental::filesystem::exists(f) || !std::experimental::filesystem::is_directory(f)) {
				std::cout << f << " not exists!";
				ExitProcess(1);
			}
		}
	}
	catch (const CLI::ParseError &e) {
		//cout << e.what() << endl;
		int ret = app->exit(e);
		ExitProcess(ret);
	}
	delete args;
}

string Config::get(string key) {
	auto intValue = argvFlagMap.find(key);
	if (intValue == argvFlagMap.end()) return "";
	return intValue->second;// std::to_string(intValue->second);
}

