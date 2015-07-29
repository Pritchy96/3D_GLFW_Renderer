#pragma once
#include "stdafx.h"
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Logger
{
public:
	Logger();
	~Logger();

	static void log(string text, bool truncate, bool log_to_console);
	static void log_multiple(vector<string> text, bool truncate, bool log_to_console);
};

