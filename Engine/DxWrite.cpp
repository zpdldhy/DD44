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
	
	//폰트등록
	auto font = AddFontResource(L"../Resources/Font/font6.ttf");
	font = AddFontResource(L"../Resources/Font/font5.ttf");
	SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	
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

// 동적으로 폰트 변경.
void DxWrite::SetFont(const std::wstring& _fontName, float _fontSize)
{
	m_wsFontName = _fontName;
	m_fFontSize = _fontSize;

	if (!m_pDxWrite)
	{
		return;
	}

	//m_pTextFormat.ReleaseAndGetAddressOf()을 통해서 이전 포맷을 안전하게 해제 후 새포인터 할당.
	HRESULT hr = m_pDxWrite->CreateTextFormat(
		_fontName.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		_fontSize,
		L"ko-kr",
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
	SetFont(m_wsFontName, m_fFontSize);
}

void DxWrite::DecreaseFontSize(float _step)
{
	m_fFontSize -= _step;
	SetFont(m_wsFontName, m_fFontSize);
}

void DxWrite::DirectDraw(D2D1_RECT_F layoutRect, std::wstring msg, D2D1::ColorF Color)
{
	m_pd2dRT->BeginDraw();
	if (m_pd2dRT)
	{
		m_pColorBrush->SetColor(Color);
		m_pd2dRT->DrawText(msg.c_str(), msg.size(), m_pTextFormat.Get(), &layoutRect, m_pColorBrush.Get());
	}
	m_pd2dRT->EndDraw();
}

void DxWrite::Draw(D2D1_RECT_F layoutRect, std::wstring msg, D2D1::ColorF Color)
{
	if (m_pd2dRT)
	{
		m_pColorBrush->SetColor(Color);
		m_pd2dRT->DrawText(msg.c_str(), msg.size(), m_pTextFormat.Get(), &layoutRect, m_pColorBrush.Get());
	}
}

