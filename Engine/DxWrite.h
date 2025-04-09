#pragma once
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <string>
#include <list>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#include "Singleton.h"

class DxWrite : public Singleton<DxWrite>
{
public:
	ComPtr<ID2D1Factory>			m_pd2dFactory;
	ComPtr<ID2D1RenderTarget>		m_pd2dRT;
	ComPtr<IDWriteFactory>			m_pDxWrite;
	ComPtr<IDWriteTextFormat>		m_pTextFormat;
	ComPtr<ID2D1SolidColorBrush>	m_pColorBrush;

public:
	HRESULT   Create();

public:
	float m_fFontSize = 20.0f;
	wstring m_wsFontName = L"";

public:
	void SetFont(const std::wstring& _fontName, float _fontSize);
	void IncreaseFontSize(float _step = 2.0f);
	void DecreaseFontSize(float _step = 2.0f);


public:
	void   DirectDraw(D2D1_RECT_F layoutRect, std::wstring msg, D2D1::ColorF Color = D2D1::ColorF(1, 1, 1, 1));
	void   Draw(D2D1_RECT_F layoutRect, std::wstring msg, D2D1::ColorF Color = D2D1::ColorF(1, 0, 1, 1));
};

