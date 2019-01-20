#pragma once
#include <string>
#include <map>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#include "CLI11.hpp"
using std::string;

class Config {
private:
	static std::map<string, string> argvFlagMap;
	static CLI::App* app;
	static void insert(string registry, string option, string defaultValue, string helpString = "");

public:

	static std::vector<std::string> scanList;
	static std::string outputPath;
	static void initialize();
	static std::string get(std::string);
};