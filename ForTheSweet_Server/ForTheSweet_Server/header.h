#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>

using namespace std;
using namespace chrono;

#include <winsock2.h>
#include <windows.h>

#include <MMSystem.h>					// timer.cpp�� �ʿ�(timeGetTime)
#include <tchar.h>

#pragma comment(lib, "winmm.lib")	// timer.cpp�� �ʿ�(timeGetTime)