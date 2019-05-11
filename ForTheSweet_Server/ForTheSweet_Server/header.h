#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

using namespace std;

#include <winsock2.h>
#include <windows.h>

#include <MMSystem.h>					// timer.cpp에 필요(timeGetTime)
#include <tchar.h>

#pragma comment(lib, "winmm.lib")	// timer.cpp에 필요(timeGetTime)