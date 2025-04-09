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

	//���丮 ����
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_pd2dFactory.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return hr;
	}

	//dpi ���� : ȭ�鿡 �ѿ��� ������ �ʰ� ����.
	UINT dpi = GetDpiForWindow(g_hWnd);

	// UINT Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // dwrite���� �ʼ�
	D2D1_RENDER_TARGET_PROPERTIES rtp;
	ZeroMemory(&rtp, sizeof(rtp));
	rtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	//3D�� ���� �����ϰ� PREMULTIPLIED ����.
	rtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
	rtp.dpiX = dpi;
	rtp.dpiY = dpi;

	//D3D11 ����۱������ D2D ����Ÿ���� ����.
	hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(pBackBuffer.Get(), &rtp, m_pd2dRT.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return hr;
	}
	//�귯�� ����.
	m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), m_pColorBrush.GetAddressOf());

	//DirectWrite ���丮 ����.
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)m_pDxWrite.GetAddressOf());
	
	//��Ʈ���
	auto font = AddFontResource(L"../Resources/Font/font6.ttf");
	font = AddFontResource(L"../Resources/Font/font5.ttf");
	SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	
	//�ʱ� ��Ʈ, ũ��
	m_wsFontName = L"Mapoȫ��������";
	m_fFontSize = 20;


	if (SUCCEEDED(hr))
	{
		//�ؽ�Ʈ ���˼���.
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

// �������� ��Ʈ ����.
void DxWrite::SetFont(const std::wstring& _fontName, float _fontSize)
{
	m_wsFontName = _fontName;
	m_fFontSize = _fontSize;

	if (!m_pDxWrite)
	{
		return;
	}

	//m_pTextFormat.ReleaseAndGetAddressOf()�� ���ؼ� ���� ������ �����ϰ� ���� �� �������� �Ҵ�.
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

