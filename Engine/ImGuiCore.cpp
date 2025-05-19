#include "pch.h"
#include "ImGuiCore.h"
#include "Device.h"
#include "Input.h"

void ImGuiCore::Init()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
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
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(DEVICE.Get(), DC.Get());

	m_pCharacterEditorUI = make_unique<CharacterEditorUI>();
	m_pMapEditorUI = make_unique<MapEditorUI>();
	m_pObjectEditorUI = make_unique<ObjectEditorUI>();
	m_pEffectEditorUI = make_unique<EffectEditorUI>();
	m_pActorListUI = std::make_unique<ActorListUI>();
	m_pMeshEditorUI = make_unique<MeshEditorUI>();
}

void ImGuiCore::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	ImGuiIO& io = ImGui::GetIO();

	// Press 'F1' key to pop-up UI Tool
	if (INPUT->GetButton(GameKey::F1))
	{
		m_bEditorToolVisible = !m_bEditorToolVisible;
	}

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
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiCore::Release()
{

}
