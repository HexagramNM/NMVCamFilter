#pragma once

#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM 

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4995)
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#pragma comment(lib, "strmbasd.lib")
#else
#pragma comment(lib, "strmbase.lib")
#endif

#include <streams.h>
#include <initguid.h>
#include <wchar.h>

#include <math.h>
//参考ページ

//32bitと64bitの問題
//https://docs.microsoft.com/ja-jp/cpp/build/common-visual-cpp-64-bit-migration-issues?view=vs-2019

//skype等で認識されるには、Pin側のクラスにIKsPropertySetとIAMStreamConfig、IAMFilterMiscFlagsのインタフェースが必要
//これらを追加する際にIUnknownインタフェースのメソッドを実装する必要もあり。
//https://community.osr.com/discussion/245023/virtual-camera-source-filter-directshow
//https://docs.microsoft.com/ja-jp/office/client-developer/outlook/mapi/implementing-iunknown-in-c-plus-plus

//仮想カメラに必要なインタフェースの実装例
//https://github.com/aricatamoy/svcam

//DirectShowFilterカテゴリに配置する登録処理
//https://docs.microsoft.com/en-us/windows/win32/directshow/implementing-dllregisterserver

//他ウィンドウの取得
//http://7ujm.net/C++/EnumWindows.html

//別スレッドでGetWindowTextをするとデッドロック状態になることがあるらしい
//http://yu1rows.blogspot.com/2014/05/getwindowtext.html

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#define TEMPLATE_NAME	(L"NM Virtual Cam Filter")
#define FILTER_NAME		(TEMPLATE_NAME)
#define OUTPUT_PIN_NAME (L"Output")
#define TARGET_WINDOW_NAME (L"_NM_MicDisplay_")

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define PIXEL_BIT 24

#include "NMVCamFilter.h"




