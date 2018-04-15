// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <objbase.h>
#include <zmouse.h>

#include "..\DuiLib\UIlib.h"
using namespace DuiLib;

#ifdef _DEBUG
#       pragma comment(lib, "..\\Bin\\DuiLib_d.lib")
#else
#       pragma comment(lib, "..\\Bin\\DuiLib.lib")
#endif