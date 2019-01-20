#pragma once
#include "stdafx.h"
#include "config.h"
#include <string>
#include <vector>
#include <iostream>
#include <map>

std::map<string, int> Config::argvFlagMap;
CLI::App* Config::app;
std::vector<std::string> Config::scanList;

void Config::insert(string registry, string option, int defaultValue, string helpString) {
	argvFlagMap[registry] = defaultValue;
	app->add_flag(option, argvFlagMap[registry], helpString);
}

void Config::initialize() {
	app = new CLI::App{ "WebShellKill CLI" };
	LPWSTR *szArglist;
	int nArgs;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

	std::string filename = "default";

	insert("Check_Hide_File", "-i,--show-hidden-file", 1);
	insert("Show_Zend_File", "-z,--show-zend-file", 1);
	insert("Auto_Check_New_Ver", "-a,--auto-check-new-version", 1);
	insert("Hide_Levl1", "-d,--hide-level1", 1);
	insert("Check_Type", "-t,--type", 1);
	insert("Show_Hide_dir_", "-e,--exclude-dir", 0);

	app->allow_extras(true);

	const char** args = new const char*[nArgs];
	for (auto i = 0; i < nArgs; i++) {
		size_t len = lstrlenW(szArglist[i]);
		char* arg = new char[len * 2];
		sprintf_s(arg, len + 1, "%ls", szArglist[i]);
		args[i] = arg;
		printf("%d %s\n", strlen(arg), arg);
	}


	try {
		app->parse(nArgs, args);
		scanList = app->remaining();
	}
	catch (const CLI::ParseError &e) {
		printf("%s\n", e.what());
		int ret = app->exit(e);
		ExitProcess(ret);
	}
	delete args;
}
