#include "pch.h"
#include "DxWrite.h"
#include "Device.h"
#include "Text.h"

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
	rtp.dpiX = static_cast<FLOAT>(dpi);
	rtp.dpiY = static_cast<FLOAT>(dpi);

	//D3D11 백버퍼기반으로 D2D 렌더타겟을 생성.
	hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(pBackBuffer.Get(), &rtp, m_pd2dRT.GetAddressOf());

	if (FAILED(hr))
	{
		if (IsRenderDocPresent())
		{
			OutputDebugString(L"[Warning] RenderDoc 실행 중: D2D RenderTarget 생성 실패 무시.\n");
			m_pd2dRT.Reset(); // 사용하지 않도록 무효화
			return S_OK;       // 실패하지 않도록 우회
		}
		else
		{
			DX_CHECK(hr, _T(__FUNCTION__));
			return hr;
		}
	}

	m_vFontPaths.push_back(L"");

	//DirectWrite 팩토리 생성
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)m_pDxWrite.GetAddressOf());

	// Main 폰트 생성
	m_pMainFont = make_shared<Font>();
	m_pMainFont->Create(L"", 20.f, Color(1.f, 0.f, 0.f, 1.f));

	return hr;
}

bool DxWrite::LoadFontResources(const wstring& _szFontPath)
{
	for (auto& font : m_vFontPaths)
		if (font == _szFontPath)
			return false;
			
	m_vFontPaths.push_back(_szFontPath);
	wstring ext = SplitPath(_szFontPath);
	int added = AddFontResource(_szFontPath.c_str());
	if (added > 0)
	{
		return true;
	}

	assert(0 && "Failed to load font resource");

	return false;
}

//폰트 등록 함수.
bool DxWrite::LoadFontResources(const vector<wstring>& _szFontPaths)
{
	for (const auto& path : _szFontPaths)
	{
		if (!LoadFontResources(path))
		{
			wstring str = L"[DxWrite] Failed to load font resource: " + path + L"\n";
			OutputDebugString(str.c_str());
		}

		return false;
	}
	return true;
}

bool DxWrite::IsRenderDocPresent()
{
	HMODULE hMod = GetModuleHandleW(L"renderdoc.dll");
	m_bUseRenderDoc = true;
	return (hMod != nullptr);
}

void DxWrite::BeginDraw()
{
	if (!m_bUseRenderDoc)
		m_pd2dRT->BeginDraw();
}

void DxWrite::Draw(D2D1_RECT_F _layoutRect, std::wstring _msg)
{
	if (m_pd2dRT && !m_bUseRenderDoc)
	{
		//m_pColorBrush->SetColor(Color);
		m_pd2dRT->DrawText(
			_msg.c_str(),
			static_cast<UINT32>(_msg.size()), 
			m_pMainFont->GetTextFormat(), &_layoutRect,
			m_pMainFont->GetBrush());
	}
}

void DxWrite::EndDraw()
{
	if (!m_bUseRenderDoc)
		m_pd2dRT->EndDraw();
}

////발광효과 글씨표현.
//void DxWrite::DrawGlow(D2D1_RECT_F rect, std::wstring msg, D2D1::ColorF glowColor, D2D1::ColorF mainColor)
//{
//	if (!m_pd2dRT || !m_pTextFormat) return;
//
//	const float glowOffsets[] = { -1, 0, 1 };
//
//	for (float dx : glowOffsets)
//	{
//		for (float dy : glowOffsets)
//		{
//			if (dx == 0 && dy == 0) continue;
//
//			D2D1_RECT_F glowRect = rect;
//			glowRect.left += dx;
//			glowRect.top += dy;
//			glowRect.right += dx;
//			glowRect.bottom += dy;
//
//			m_pColorBrush->SetColor(glowColor);
//			m_pd2dRT->DrawText(msg.c_str(), static_cast<UINT32>(msg.size()), m_pTextFormat.Get(), &glowRect, m_pColorBrush.Get());
//		}
//	}
//
//	// 중앙 본 텍스트
//	m_pColorBrush->SetColor(mainColor);
//	m_pd2dRT->DrawText(msg.c_str(), static_cast<UINT32>(msg.size()), m_pTextFormat.Get(), &rect, m_pColorBrush.Get());
//}