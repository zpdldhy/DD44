#pragma once

#include "Functions.h"
#include "Types.h"
#include "Define.h"
#include "Vertex.h"

// WIN
#include <windows.h>
#include <wrl.h>
using namespace Microsoft::WRL;

// STL
#include <memory>
#include <tchar.h> 
#include <vector>
#include <map>
#include <list>
#include <array>
using namespace std;

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

// ImGUI
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// LIBS
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#ifndef _DEBUG
#pragma comment(lib, "DxTk/DirectXTK_r.lib")
#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#else
#pragma comment(lib, "DxTk/DirectXTK_d.lib")
#pragma comment(lib, "DirectXTex/DirectXTex_debug.lib")
#endif

// Window º¯¼ö
extern POINT g_windowSize;
extern HWND g_hWnd;
