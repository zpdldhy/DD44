#include "pch.h"
#include "ImGuiCore.h"
#include "Device.h"
#include "Input.h"

MONITORINFOEX GetSecondMonitorInfo()
{
	std::vector<MONITORINFOEX> monitors;
	EnumDisplayMonitors(nullptr, nullptr,
		[](HMONITOR hMon, HDC, LPRECT, LPARAM data) -> BOOL {
			auto& out = *reinterpret_cast<std::vector<MONITORINFOEX>*>(data);
			MONITORINFOEX mi = {};
			mi.cbSize = sizeof(mi);
			GetMonitorInfo(hMon, &mi);
			out.push_back(mi);
			return TRUE;
		}, reinterpret_cast<LPARAM>(&monitors));

	if (monitors.size() >= 2)
		return monitors[1];  // 두 번째 모니터
	else
		return monitors[0];  // 모니터가 하나뿐이면 첫 번째 사용
}

void ImGuiCore::Init()
{
	CreateDevice();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.FontGlobalScale = dpiScale;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// Setup Dear ImGui style
	if (m_bDark == true)
	{
		ImGui::StyleColorsDark();
	}
	else
	{
		ImGui::StyleColorsLight();
	}

	// Setup Platform/Renderer backends

	if (m_hWndImGui)
		ImGui_ImplWin32_Init(m_hWndImGui);
	else
		ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pd3dDevice.Get(), m_pd3dContext.Get());

	m_pCharacterEditorUI = make_unique<CharacterEditorUI>();
	m_pMapEditorUI = make_unique<MapEditorUI>();
	m_pObjectEditorUI = make_unique<ObjectEditorUI>();
	m_pEffectEditorUI = make_unique<EffectEditorUI>();
	m_pActorListUI = std::make_unique<ActorListUI>();
	m_pMeshEditorUI = make_unique<MeshEditorUI>();
	m_pUIEditorUI = make_unique<UIEditorUI>();
	m_pParticleEditorUI = make_unique<ParticleEditorUI>();
}

void ImGuiCore::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	ImGuiIO& io = ImGui::GetIO();

	// Press 'F1' key to pop-up UI Tool
	if (INPUT->GetButton(GameKey::F1))
		m_bEditorToolVisible = !m_bEditorToolVisible;	

	if(INPUT->GetButton(GameKey::F2))
		g_bRangeVisibleMode = !g_bRangeVisibleMode;

	if (m_bEditorToolVisible)
	{
		float panelWidth = 400.0f;
		ImVec2 panelPos = ImVec2(io.DisplaySize.x - panelWidth, 0.0f);
		ImVec2 panelSize = ImVec2(panelWidth, io.DisplaySize.y);

		ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(panelSize, ImGuiCond_Always);

		ImGui::Begin("Editor Tool", nullptr,
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse);

		if (ImGui::BeginTabBar("EditorTabs"))
		{
			if (ImGui::BeginTabItem("Character Editor"))
			{
				if (m_pCharacterEditorUI) m_pCharacterEditorUI->DrawUI();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Map Editor"))
			{
				if (m_pMapEditorUI) m_pMapEditorUI->DrawUI();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Object Editor"))
			{
				if (m_pObjectEditorUI) m_pObjectEditorUI->DrawUI();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Effect Editor"))
			{
				if (m_pEffectEditorUI) m_pEffectEditorUI->DrawUI();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Mesh Editor"))
			{
				if (m_pMeshEditorUI) m_pMeshEditorUI->DrawUI();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("UI Editor"))
			{
				if (m_pUIEditorUI) m_pUIEditorUI->DrawUI();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Particle Editor"))
			{
				if (m_pParticleEditorUI) m_pParticleEditorUI->DrawUI();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(10, io.DisplaySize.y - 50), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(140, 40), ImGuiCond_Always);

		ImGui::Begin("ActorListButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		if (ImGui::Button("Actor List", ImVec2(-1, -1)))
		{
			m_pActorListUI->Toggle();
		}

		ImGui::End();

		if (m_pActorListUI)
		{
			m_pActorListUI->DrawUI();
		}
	}
}
 
void ImGuiCore::Render()
{
	PreRender();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	PostRender();
}

void ImGuiCore::Release()
{

}

bool ImGuiCore::CreateImGuiWindow(HINSTANCE _hInstance, int _winX, int _winY)
{
	MONITORINFOEX secondMonitor = GetSecondMonitorInfo();
	RECT rc = secondMonitor.rcMonitor;
	int x = rc.left;
	int y = rc.top;

	m_hWndImGui = CreateWindow(
		L"DD44", L"Tool", WS_OVERLAPPEDWINDOW,
		x, y, _winX, _winY, nullptr, nullptr, _hInstance, nullptr);

	GetClientRect(m_hWndImGui, &rc);

	m_iWidth = rc.right - rc.left;
	m_iHeight = rc.bottom - rc.top;

	dpiScale = (float)m_iHeight / _winY;

	if (!m_hWndImGui)
	{
		return false;
	}

	ShowWindow(m_hWndImGui, SW_SHOW);
	UpdateWindow(m_hWndImGui);
}

bool ImGuiCore::CreateDevice()
{
	if (!m_hWndImGui)
		return false;

	// Device & SwapChain 생성
	D3D_FEATURE_LEVEL pFeatureLevel;
	IDXGIAdapter* pAdapter = nullptr;
	D3D_DRIVER_TYPE DriverType = D3D_DRIVER_TYPE_HARDWARE;
	HMODULE Software = NULL;
	UINT Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;
	UINT FeatureLevels = 1;
	UINT SDKVersion = D3D11_SDK_VERSION;

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferDesc.Width = m_iWidth;
	scd.BufferDesc.Height = m_iHeight;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 2;
	scd.OutputWindow = m_hWndImGui;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;        // Win8 이상 호환성 높임
	scd.SampleDesc.Count = 1;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		pAdapter,
		DriverType,
		Software,
		Flags,
		&pFeatureLevels,
		FeatureLevels,
		SDKVersion,
		&scd,
		m_pSwapChain.GetAddressOf(),
		m_pd3dDevice.GetAddressOf(),
		&pFeatureLevel,
		m_pd3dContext.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateDeviceAndSwapChain"));
		return false;
	}

	// Render Target View 생성
	ComPtr<ID3D11Texture2D> pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return false;
	}

	hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, m_pRTV.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return false;
	}

	m_MainVP.Width = (FLOAT)m_iWidth;
	m_MainVP.Height = (FLOAT)m_iHeight;
	m_MainVP.MinDepth = 0.0f;
	m_MainVP.MaxDepth = 1.0f;
	m_MainVP.TopLeftX = 0.0f;
	m_MainVP.TopLeftY = 0.0f;

	return true;
}

POINT ImGuiCore::GetMousePos()
{
	GetCursorPos(&m_pMousePos);
	ScreenToClient(m_hWndImGui, &m_pMousePos);
	return m_pMousePos;
}

void ImGuiCore::PreRender()
{
	m_pd3dContext->RSSetViewports(1, &m_MainVP);
	m_pd3dContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), nullptr);

	float ClearColor[] = { 0.5f, 0.5f, 0.5f, 1.f };

	m_pd3dContext->ClearRenderTargetView(m_pRTV.Get(), ClearColor);
}

void ImGuiCore::PostRender()
{
	m_pSwapChain->Present(0, 0);
}
