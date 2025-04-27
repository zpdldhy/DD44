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
	std::vector<std::wstring> m_vFontPaths;

public:
	void LoadFontResources(const vector<wstring>& fontPaths);
	void SetFont(const wstring& _fontPath);
	void IncreaseFontSize(float _step = 2.0f);
	void DecreaseFontSize(float _step = 2.0f);
	void SetFontColor(D2D1::ColorF _color);
	void SetAlignment(DWRITE_TEXT_ALIGNMENT _textAlign, DWRITE_PARAGRAPH_ALIGNMENT _paraAlign);


public:
	void   DirectDraw(D2D1_RECT_F _layoutRect, std::wstring _msg);
	void   Draw(D2D1_RECT_F _layoutRect, std::wstring _msg);
	void   DrawMultiline(D2D1_RECT_F _layoutRect, std::wstring _msg);
	void   DrawGlow(D2D1_RECT_F rect, std::wstring msg, D2D1::ColorF glowColor, D2D1::ColorF mainColor);
};

class Typer : public Singleton<Typer>
{
private:
	std::wstring fullText;
	std::wstring currentText;
	float m_fCharDelay = 0.05f;
	float m_fTimer = 0.0f;
	size_t visibleLength = 0;
	bool m_bFinished = false;

public:
	void Reset(const std::wstring& newText);
	void Update(float deltaTime);
	void Draw(D2D1_RECT_F rect);
};