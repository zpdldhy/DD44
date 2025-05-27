#pragma once
#include "Singleton.h"

class Font;

class DxWrite : public Singleton<DxWrite>
{
public:
	ComPtr<ID2D1Factory>			m_pd2dFactory = nullptr;
	ComPtr<ID2D1RenderTarget>		m_pd2dRT = nullptr;
	ComPtr<IDWriteFactory>			m_pDxWrite = nullptr;

	shared_ptr<class Font>			m_pMainFont = nullptr;
	vector<wstring> 				m_vFontPaths;

public:
	HRESULT   Create();

private:
	bool m_bUseRenderDoc = false;

public:
	bool LoadFontResources(const wstring& fontPaths);
	bool LoadFontResources(const vector<wstring>& fontPaths);

private:
	bool IsRenderDocPresent();

public:
	void BeginDraw();
	void Draw(D2D1_RECT_F _layoutRect, std::wstring _msg);
	void EndDraw();
	//void DrawGlow(D2D1_RECT_F rect, std::wstring msg, D2D1::ColorF glowColor, D2D1::ColorF mainColor);
};