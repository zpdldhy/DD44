#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <tchar.h> 
#include <locale>
#include <codecvt>

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

static std::wstring SplitName(std::wstring filename)
{
	TCHAR szFileName[256];
	TCHAR Drive[MAX_PATH];
	TCHAR Dir[MAX_PATH];
	TCHAR FName[MAX_PATH];
	TCHAR Ext[MAX_PATH];
	_tsplitpath_s(filename.c_str(), Drive, Dir, FName, Ext);

	std::wstring key = FName;
	return key;
}

static std::wstring to_mw(const std::string& _src)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(_src);
}

static std::string to_wm(const std::wstring& _src)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(_src);
}

static std::string ReplaceAsterisk(const std::string& pathTemplate, const std::string& filename) {
	size_t pos = pathTemplate.find('*');
	if (pos != std::string::npos) {
		std::string result = pathTemplate;
		result.replace(pos, 1, filename);
		return result;
	}
	return pathTemplate; // '*'가 없으면 그대로 반환
}

// 폴더 내부 모든 파일 이름 가져오기
// _path : "../Resources/fbx/*.fbx"
static std::vector<std::string> GetFileNames(std::string _path)
{
	HANDLE hFind;
	WIN32_FIND_DATAA data;
	std::vector<std::string> fileList;
	if ((hFind = FindFirstFileA(_path.c_str(), &data)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::string name = to_wm(SplitName(to_mw(data.cFileName)));
			fileList.emplace_back(ReplaceAsterisk(_path, name));
		} while (FindNextFileA(hFind, &data) != 0);
		FindClose(hFind);
	}
	return fileList;
}
