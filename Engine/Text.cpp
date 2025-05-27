#include "pch.h"
#include "Text.h"
#include "DxWrite.h"

HRESULT Font::Create(const wstring& _szFontPath, float _fFontSize, Color _color)
{
	DXWRITE->LoadFontResources(_szFontPath);

	wstring name = m_szFontName = SplitName(_szFontPath);
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
	if (m_pTextFormat)
	{
		m_pTextFormat.Reset();
		m_pTextFormat = nullptr; // Reset the smart pointer
	}

	//텍스트 포맷 생성
	HRESULT hr = DXWRITE->m_pDxWrite->CreateTextFormat(
		_szFontName.c_str(), NULL,
		DWRITE_FONT_WEIGHT_THIN,
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

void Font::SetSize(float _size)
{
	m_fFontSize = _size;
	CreateTextFormat(m_szFontName, m_fFontSize);
}

void Font::IncreaseSize(float _step)
{
	m_fFontSize += _step;
	CreateTextFormat(m_szFontName, m_fFontSize);
}

void Font::DecreaseSize(float _step)
{
	m_fFontSize -= _step;
	CreateTextFormat(m_szFontName, m_fFontSize);
}

void Font::SetColor(Color _color)
{
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

HRESULT Text::Create(const wstring& _msg, const Vec2& _size, shared_ptr<Font> _font)
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
	// 색상 설정 및 그리기
	DXWRITE->m_pd2dRT->DrawTextLayout(
		{ _position.x, _position.y },
		m_pTextLayout.Get(),
		m_pFont->GetBrush());
}

void Text::SetFont(shared_ptr<Font> font)
{
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
	m_szMessage = msg;
	Create(m_szMessage, m_vSize, m_pFont);
}

void Text::SetSize(const Vec2& size)
{
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
