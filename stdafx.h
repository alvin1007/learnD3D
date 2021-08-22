#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용은 windows 헤더에서 제외

// Windows 헤더 파일
#ifndef WINDOW
#include <windows.h>
#define WINDOW
#endif

// C 런타임 헤더 파일
#include <cstdio>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <sstream>

// TODO : 프로그램에 필요한 추가 헤더 파일은 여기에 선언
#include "DxDefine.h"