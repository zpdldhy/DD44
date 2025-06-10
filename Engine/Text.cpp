#include "pch.h"
#include "Text.h"
#include "DxWrite.h"

HRESULT Font::Create(const wstring& _szFontPath, float _fFontSize, Color _color)
{
	if (!DXWRITE->m_pDxWrite)
		return false;

	DXWRITE->LoadFontResources(_szFontPath);

	wstring name = m_szFontPath = _szFontPath;
	m_fFontSize = _fFontSize;
	m_color = _color;

	CreateTextFormat(name, _fFontSize);

	if (m_pColorBrush)
	{
		m_pColorBrush.Reset();
		m_pColorBrush = nullptr; // Reset the smart pointer
	}

	// 브러쉬 생성
	HRESULT hr = DXWRITE->m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(_color.x, _color.y, _color.z, _color.w), m_pColorBrush.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return hr;
	}

	return hr;
}

HRESULT Font::CreateTextFormat(const wstring& _szFontName, float _fFontSize)
{
	if (!DXWRITE->m_pDxWrite)
		return false;

	if (m_pTextFormat)
	{
		m_pTextFormat.Reset();
		m_pTextFormat = nullptr; // Reset the smart pointer
	}

	//텍스트 포맷 생성
	HRESULT hr = DXWRITE->m_pDxWrite->CreateTextFormat(
		_szFontName.c_str(), NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		_fFontSize, L"ko-kr",
		m_pTextFormat.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return hr;
	}

	return hr;
}

void Font::SetPath(wstring _path)
{
	if (m_szFontPath == _path)
		return;

	m_szFontPath = _path;
	CreateTextFormat(m_szFontPath, m_fFontSize);
}

void Font::SetSize(float _size)
{
	if (m_fFontSize == _size)
		return;

	m_fFontSize = _size;
	CreateTextFormat(m_szFontPath, m_fFontSize);
}

void Font::IncreaseSize(float _step)
{
	m_fFontSize += _step;
	CreateTextFormat(m_szFontPath, m_fFontSize);
}

void Font::DecreaseSize(float _step)
{
	m_fFontSize -= _step;
	CreateTextFormat(m_szFontPath, m_fFontSize);
}

void Font::SetColor(Color _color)
{
	if (m_color == _color)
		return;

	m_color = _color;
	m_pColorBrush->SetColor(D2D1::ColorF(_color.x, _color.y, _color.z, _color.w));
}

void Font::SetAlignment(DWRITE_TEXT_ALIGNMENT _textAlign, DWRITE_PARAGRAPH_ALIGNMENT _paraAlign)
{
	if (m_pTextFormat)
	{
		m_pTextFormat->SetTextAlignment(_textAlign);
		m_pTextFormat->SetParagraphAlignment(_paraAlign);
	}
}

Font::~Font()
{
	if (m_pTextFormat)
	{
		m_pTextFormat.Reset();
		m_pTextFormat = nullptr; // Reset the smart pointer
	}
	if (m_pColorBrush)
	{
		m_pColorBrush.Reset();
		m_pColorBrush = nullptr; // Reset the smart pointer
	}
}

HRESULT Text::Create(const wstring& _msg, const Vec2& _size, Font* _font)
{
	if (m_pTextLayout)
	{
		m_pTextLayout.Reset();
		m_pTextLayout = nullptr; // Reset the smart pointer
	}

	if (!_font)
	{
		OutputDebugString(L"[Text] Font is null.\n");
		return E_FAIL;
	}

	m_pFont = _font;
	m_szMessage = _msg;
	m_vSize = _size;

	// 텍스트 레이아웃 생성
	HRESULT hr = DXWRITE->m_pDxWrite->CreateTextLayout(
		_msg.c_str(),
		static_cast<UINT32>(_msg.length()),
		_font->GetTextFormat(),
		_size.x, _size.y,
		m_pTextLayout.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return hr;
	}

	return hr;
}

void Text::Draw(const Vec2& _position)
{
	if (m_pFont == nullptr || m_pTextLayout == nullptr)
		return;

	// 색상 설정 및 그리기
	DXWRITE->m_pd2dRT->DrawTextLayout(
		{ _position.x - (m_vSize.x / 2.f),
		_position.y - (m_vSize.y / 2.f) },
		m_pTextLayout.Get(),
		m_pFont->GetBrush());
}

void Text::SetFont(Font* font)
{
	if (m_pFont == font)
		return;

	if (!font)
	{
		OutputDebugString(L"[Text] Font is null.\n");
		return;
	}

	m_pFont = font;
	Create(m_szMessage, m_vSize, m_pFont);
}

void Text::SetMessage(const wstring& msg)
{
	if (m_szMessage == msg)
		return;

	m_szMessage = msg;
	Create(m_szMessage, m_vSize, m_pFont);
}

void Text::SetSize(const Vec2& size)
{
	if (m_vSize == size)
		return;

	m_vSize = size;
	Create(m_szMessage, m_vSize, m_pFont);
}

Text::~Text()
{
	if (m_pTextLayout)
	{
		m_pTextLayout.Reset();
		m_pTextLayout = nullptr; // Reset the smart pointer
	}
}

//void Typer::Reset(const std::wstring& newText)
//{
//	fullText = newText;
//	currentText.clear();
//	visibleLength = 0;
//	m_fTimer = 0.0f;
//	m_bFinished = false;
//}
//
//void Typer::Update(float deltaTime)
//{
//	if (m_bFinished) return;
//
//	m_fTimer += deltaTime;
//	while (m_fTimer >= m_fCharDelay && visibleLength < fullText.size())
//	{
//		m_fTimer -= m_fCharDelay;
//		visibleLength++;
//		currentText = fullText.substr(0, visibleLength);
//
//		if (visibleLength == fullText.size())
//		{
//			m_bFinished = true;
//		}
//	}
//}
//
//void Typer::Draw(D2D1_RECT_F rect)
//{
//	DXWRITE->DrawMultiline(rect, currentText);
//}