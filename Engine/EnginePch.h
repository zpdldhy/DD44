#pragma once

#include "Functions.h"
#include "Types.h"
#include "Define.h"
#include "Vertex.h"
#include "ShapeData.h"

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
#include <algorithm>
using namespace std;

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

// Write2D
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

// ImGUI
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// Assimp
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include <Assimp/cimport.h>

// JSON
#include "JSON/json.hpp"
using json = nlohmann::json;

// LIBS
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#ifndef _DEBUG
#pragma comment(lib, "DxTk/DirectXTK_r.lib")
#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#pragma comment(lib, "Assimp/assimp-vc143-mt.lib")
#else
#pragma comment(lib, "DxTk/DirectXTK_d.lib")
#pragma comment(lib, "DirectXTex/DirectXTex_debug.lib")
#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")
#endif

// Window º¯¼ö
extern POINT g_windowSize;
extern HWND g_hWnd;
extern bool bRunGame;
