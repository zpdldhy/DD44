#pragma once
#include <windows.h>
#include <string>
#include <tchar.h> 

static void DX_CHECK(HRESULT _hr, const TCHAR* _function)
{
	if (FAILED(_hr))
	{
		LPWSTR output;
		WCHAR buffer[256] = { 0, };
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL, _hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&output, 0, NULL);
		wsprintf(buffer, L"File=%ls\nLine=%d\nFuction=%ls", _T(__FILE__), __LINE__, _function);
		std::wstring msg = buffer;
		msg += L"\n";
		msg += output;

		std::wstring title = L"ERROR(";
		title += std::to_wstring(_hr);
		title += L")";
		MessageBox(NULL, msg.c_str(),
			title.c_str(), MB_OK);
	}
}

// 추후 수정 필요
static std::wstring SplitPath(std::wstring _filename)
{
	TCHAR szFileName[256];
	TCHAR Drive[MAX_PATH];
	TCHAR Dir[MAX_PATH];
	TCHAR FName[MAX_PATH];
	TCHAR Ext[MAX_PATH];
	_tsplitpath_s(_filename.c_str(), Drive, Dir, FName, Ext);

	std::wstring key = FName;
	key += Ext;
	return key;
}