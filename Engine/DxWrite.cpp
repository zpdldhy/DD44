#include "pch.h"
#include "DxWrite.h"
#include "Device.h"

HRESULT DxWrite::Create()
{
	ComPtr<IDXGISurface> pBackBuffer;
	HRESULT hr = SWAPCHAIN->GetBuffer(
		0, __uuidof(IDXGISurface), (LPVOID*)pBackBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return hr;
	}

	//팩토리 생성
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_pd2dFactory.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return hr;
	}

	//dpi 설정 : 화면에 뿌옇게 나오지 않게 설정.
	UINT dpi = GetDpiForWindow(g_hWnd);

	// UINT Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // dwrite연동 필수
	D2D1_RENDER_TARGET_PROPERTIES rtp;
	ZeroMemory(&rtp, sizeof(rtp));
	rtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	//3D와 연동 가능하게 PREMULTIPLIED 설정.
	rtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
	rtp.dpiX = dpi;
	rtp.dpiY = dpi;

	//D3D11 백버퍼기반으로 D2D 렌더타겟을 생성.
	hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(pBackBuffer.Get(), &rtp, m_pd2dRT.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return hr;
	}
	//브러쉬 설정.
	m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), m_pColorBrush.GetAddressOf());

	//DirectWrite 팩토리 설정.
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)m_pDxWrite.GetAddressOf());
	
	//폰트 등록
	m_vFontPaths = { L"../Resources/Font/font6.ttf",
	L"../Resources/Font/font7.ttf",
	L"../Resources/Font/font5.ttf",
	L"../Resources/Font/font4.ttf",
	L"../Resources/Font/font3.ttf",
	L"../Resources/Font/font2.ttf",
	L"../Resources/Font/font.ttf",
	L"../Resources/Font/font8.ttf" };
	LoadFontResources(m_vFontPaths);
	
	//초기 폰트, 크기
	m_wsFontName = L"Mapo홍대프리덤";
	m_fFontSize = 20;

	if (SUCCEEDED(hr))
	{
		//텍스트 포맷설정.
		hr = m_pDxWrite->CreateTextFormat(m_wsFontName.c_str(), NULL, DWRITE_FONT_WEIGHT_THIN, DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL, m_fFontSize, L"ko-kr", m_pTextFormat.GetAddressOf());

		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
			return hr;
		}
	}
	return hr;
}

//폰트 등록 함수.
void DxWrite::LoadFontResources(const vector<wstring>& fontPaths)
{
	int totalFontsAdded = 0;

	for (const auto& path : fontPaths)
	{
		int added = AddFontResource(path.c_str());
		if (added > 0)
		{
			totalFontsAdded += added;
		}
	}

	// 등록된 폰트가 하나라도 있으면 시스템에 알림
	if (totalFontsAdded > 0)
	{
		SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	}
	else
	{
		return;
	}
}

// 동적으로 폰트 변경.
void DxWrite::SetFont(const std::wstring& _fontName)
{
	m_wsFontName = _fontName;

	if (!m_pDxWrite)
	{
		return;
	}

	//m_pTextFormat.ReleaseAndGetAddressOf()을 통해서 이전 포맷을 안전하게 해제 후 새포인터 할당.
	HRESULT hr = m_pDxWrite->CreateTextFormat(
		_fontName.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		m_fFontSize,
		L"",
		m_pTextFormat.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return;
	}
}

void DxWrite::IncreaseFontSize(float _step)
{
	m_fFontSize += _step;
	SetFont(m_wsFontName);
}

void DxWrite::DecreaseFontSize(float _step)
{
	m_fFontSize -= _step;
	SetFont(m_wsFontName);
}

void DxWrite::SetFontColor(D2D1::ColorF _color)
{
	if (m_pColorBrush)
	{
		m_pColorBrush->SetColor(_color);
	}
}

void DxWrite::SetAlignment(DWRITE_TEXT_ALIGNMENT _textAlign, DWRITE_PARAGRAPH_ALIGNMENT _paraAlign)
{
	if (m_pTextFormat)
	{
		m_pTextFormat->SetTextAlignment(_textAlign);
		m_pTextFormat->SetParagraphAlignment(_paraAlign);
	}
}

void DxWrite::DirectDraw(D2D1_RECT_F _layoutRect, std::wstring _msg)
{
	m_pd2dRT->BeginDraw();
	if (m_pd2dRT)
	{
		m_pd2dRT->DrawText(_msg.c_str(), _msg.size(), m_pTextFormat.Get(), &_layoutRect, m_pColorBrush.Get());
	}
	m_pd2dRT->EndDraw();
}

void DxWrite::Draw(D2D1_RECT_F _layoutRect, std::wstring _msg)
{
	if (m_pd2dRT)
	{
		//m_pColorBrush->SetColor(Color);
		m_pd2dRT->DrawText(_msg.c_str(), _msg.size(), m_pTextFormat.Get(), &_layoutRect, m_pColorBrush.Get());
	}
}

void DxWrite::DrawMultiline(D2D1_RECT_F _layoutRect, std::wstring _msg)
{
	if (!m_pd2dRT || !m_pTextFormat || !m_pColorBrush)
		return;

	// 텍스트 레이아웃 생성
	ComPtr<IDWriteTextLayout> textLayout;
	HRESULT hr = m_pDxWrite->CreateTextLayout(
		_msg.c_str(),
		(UINT32)_msg.length(),
		m_pTextFormat.Get(),
		_layoutRect.right - _layoutRect.left,
		_layoutRect.bottom - _layoutRect.top,
		textLayout.GetAddressOf()
	);

	if (FAILED(hr))
	{
		OutputDebugString(L"[DxWrite] Failed to create TextLayout\n");
		return;
	}

	// 색상 설정 및 그리기
	m_pd2dRT->DrawTextLayout(
		{ _layoutRect.left, _layoutRect.top },
		textLayout.Get(),
		m_pColorBrush.Get()
	);
}

void Typer::Reset(const std::wstring& newText)
{
	fullText = newText;
	currentText.clear();
	visibleLength = 0;
	m_fTimer = 0.0f;
	m_bFinished = false;
}

void Typer::Update(float deltaTime)
{
	if (m_bFinished) return;

	m_fTimer += deltaTime;
	while (m_fTimer >= m_fCharDelay && visibleLength < fullText.size())
	{
		m_fTimer -= m_fCharDelay;
		visibleLength++;
		currentText = fullText.substr(0, visibleLength);

		if (visibleLength == fullText.size())
		{
			m_bFinished = true;
		}
	}
}

void Typer::Draw(D2D1_RECT_F rect)
{
	DXWRITE->DrawMultiline(rect, currentText);
}

