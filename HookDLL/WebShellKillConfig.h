#pragma once
#include <string>
#include <map>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#include "CLI11.hpp"
using std::string;

class WebShellKillConfig {
private:
	static std::map<string, int> argvFlagMap;
	static CLI::App* app;
	static std::vector<std::string> scanList;
	static void insert(string registry, string option, int defaultValue, string helpString = "");

public:
	static void initialize();
	static void parse();
};