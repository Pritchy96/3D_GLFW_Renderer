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

	void log(string text, bool truncate, bool log_to_console);
	void log_multiple(vector<string> text, bool truncate, bool log_to_console);
};

