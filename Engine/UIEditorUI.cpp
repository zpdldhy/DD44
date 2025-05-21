#include "pch.h"
#include "UIEditorUI.h"
#include "PrefabLoader.h"
#include "AUIActor.h"
#include "UMaterial.h"
#include "Texture.h"
#include "Functions.h"

void UIEditorUI::DrawUI()
{
	// ───────────────────────────── Init ─────────────────────────────

	if (m_vTextureList.empty() && m_vTextureNameList.empty())
	{
		m_vTextureList.emplace_back("../Resources/Texture/white.png");
		m_vTextureNameList.emplace_back("white.png");

		string searchPath = "../Resources/Texture/";

		// 만약 다른 형식의 파일도 필요하다면 여기에 추가
		SearchFile(searchPath, ".png");
		SearchFile(searchPath, ".jpg");
		SearchFile(searchPath, ".bmp");
	}

	// ───────────────────────────── Create ─────────────────────────────

	if (ImGui::Button("Create New UI"))
	{
		m_pUIActor = make_shared<AUIActor>();

		ResetData();

		// 초기값 세팅
		m_OnCreate(
			m_pUIActor,
			m_vTextureList[0].c_str(),	// white.png
			m_szShaderPath,
			m_Trans
		);
	}

	if (m_pUIActor == nullptr)
		return;

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	// ───────────────────────────── Transform ─────────────────────────────
	
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");

	ImGui::SameLine(0, 37.5f); ImGui::Text("%s", "X");
	ImGui::SameLine(0, 72.5f); ImGui::Text("%s", "Y");
	ImGui::SameLine(0, 72.5f); ImGui::Text("%s", "Z");

	DrawVec3("Position", m_Trans.Position);
	DrawVec3("Rotation", m_Trans.Rotation);
	DrawVec3("Scale   ", m_Trans.Scale);

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	// ───────────────────────────── Materials ─────────────────────────────
	
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Materials");

	SetTexture();

	ImGui::InputText("Shader", m_szShaderPath, IM_ARRAYSIZE(m_szShaderPath));

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	// ───────────────────────────── Actor Update ─────────────────────────────
	
	UpdateUIActor();

	// ───────────────────────────── Prefab Save/Load ─────────────────────────────
	
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Prefab Tools");

	SavePrefab();

	LoadPrefab();

	ImGui::Separator(); ImGui::Spacing();
}

void UIEditorUI::DrawVec3(const char* label, float* values)
{
	ImGui::PushID(label);
	ImGui::Text("%s", label);
	ImGui::Indent(10.0f);

	const float inputWidth = 60.0f;
	const float spacing = 20.0f;


	for (int i = 0; i < 3; ++i)
	{
		ImGui::SameLine(0, spacing);

		ImGui::PushID(i);

		ImGui::PushItemWidth(inputWidth);
		ImGui::InputFloat("##Value", &values[i], 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();		

		ImGui::PopID();
	}

	ImGui::Unindent(10.0f);
	ImGui::PopID();
}

void UIEditorUI::UpdateUIActor()
{
	if(m_pUIActor==nullptr)
		return;

	m_pUIActor->SetPosition(Vec3(m_Trans.Position));
	m_pUIActor->SetRotation(Vec3(m_Trans.Rotation));
	m_pUIActor->SetScale(Vec3(m_Trans.Scale));
}

void UIEditorUI::ResetData()
{
	// Transform
	for (int i = 0; i < 3; i++) m_Trans.Position[i] = 0.0f;
	for (int i = 0; i < 3; i++) m_Trans.Rotation[i] = 0.0f;
	for (int i = 0; i < 2; i++) m_Trans.Scale[i] = 100.0f;
	m_Trans.Scale[2] = 1.0f;

	// Texture
	m_iIdleIndex = 0;
	m_iHoverIndex = 0;
	m_iActiveIndex = 0;
	m_iSelectedIndex = 0;

	strcpy_s(m_szShaderPath, "../Resources/Shader/Default.hlsl");
}

void UIEditorUI::SetTexture()
{
	static std::vector<const char*> NamePtrs;
	if (NamePtrs.empty())
	{
		for (auto& name : m_vTextureNameList)
			NamePtrs.push_back(name.c_str());
	}

	static int iPreIdleIndex = 0;
	ImGui::Combo("Idle Texture", &m_iIdleIndex, NamePtrs.data(), (int)NamePtrs.size());
	if (m_iIdleIndex != iPreIdleIndex)
	{
		iPreIdleIndex = m_iIdleIndex;
		m_pUIActor->SetIdleTexture(TEXTURE->Get(to_mw(m_vTextureList[iPreIdleIndex])));
	}

	static int iPreHoverIndex = 0;
	ImGui::Combo("Hover Texture", &m_iHoverIndex, NamePtrs.data(), (int)NamePtrs.size());
	if (m_iHoverIndex != iPreHoverIndex)
	{
		iPreHoverIndex = m_iHoverIndex;
		m_pUIActor->SetHoverTexture(TEXTURE->Get(to_mw(m_vTextureList[iPreHoverIndex])));
	}

	static int iPreActiveIndex = 0;
	ImGui::Combo("Active Texture", &m_iActiveIndex, NamePtrs.data(), (int)NamePtrs.size());
	if (m_iActiveIndex != iPreActiveIndex)
	{
		iPreActiveIndex = m_iActiveIndex;
		m_pUIActor->SetActiveTexture(TEXTURE->Get(to_mw(m_vTextureList[iPreActiveIndex])));
	}

	static int iPreSelectedIndex = 0;
	ImGui::Combo("Selected Texture", &m_iSelectedIndex, NamePtrs.data(), (int)NamePtrs.size());
	if (m_iSelectedIndex != iPreSelectedIndex)
	{
		iPreSelectedIndex = m_iSelectedIndex;
		m_pUIActor->SetSelectTexture(TEXTURE->Get(to_mw(m_vTextureList[iPreSelectedIndex])));
	}
}

void UIEditorUI::SavePrefab()
{
	ImGui::InputText("Prefab Name", m_szPrefabName, IM_ARRAYSIZE(m_szPrefabName));

	if (ImGui::Button("Save Prefab", ImVec2(-1, 0)))
	{
		PrefabUIData data;
				
		data.Name = m_szPrefabName;

		memcpy(data.transform.Scale, m_Trans.Scale, sizeof(float) * 3);
		memcpy(data.transform.Rotation, m_Trans.Rotation, sizeof(float) * 3);
		memcpy(data.transform.Position, m_Trans.Position, sizeof(float) * 3);

		// Texture
		data.IdleTexturePath = m_vTextureNameList[m_iIdleIndex];
		data.HoverTexturePath = m_vTextureNameList[m_iHoverIndex];
		data.ActiveTexturePath = m_vTextureNameList[m_iActiveIndex];
		data.SelectedTexturePath = m_vTextureNameList[m_iSelectedIndex];

		data.ShaderPath = m_szShaderPath;

		PrefabLoader().SaveUI(data, "../Resources/Prefab/" + data.Name + ".ui.json");
	}
}

void UIEditorUI::LoadPrefab()
{
	if (ImGui::Button("Load Prefab##File", ImVec2(-1, 0)))
	{
		PrefabUIData data;
		std::string path = "../Resources/Prefab/" + std::string(m_szPrefabName) + ".ui.json";

		if (PREFAB->LoadUI(path, data))
		{
			memcpy(m_Trans.Scale, data.transform.Scale, sizeof(float) * 3);
			memcpy(m_Trans.Rotation, data.transform.Rotation, sizeof(float) * 3);
			memcpy(m_Trans.Position, data.transform.Position, sizeof(float) * 3);

			// Texture
			for (size_t i = 0; i < m_vTextureNameList.size(); ++i)
			{
				if (!strcmp(m_vTextureNameList[i].c_str(), data.IdleTexturePath.c_str()))
					m_iIdleIndex = i;
				if (!strcmp(m_vTextureNameList[i].c_str(), data.HoverTexturePath.c_str()))
					m_iHoverIndex = i;
				if (!strcmp(m_vTextureNameList[i].c_str(), data.ActiveTexturePath.c_str()))
					m_iActiveIndex = i;
				if (!strcmp(m_vTextureNameList[i].c_str(), data.SelectedTexturePath.c_str()))
					m_iSelectedIndex = i;
			}

			strcpy_s(m_szShaderPath, data.ShaderPath.c_str());
		}
	}
}

void UIEditorUI::SearchFile(const string& _directory, const string& _extension)
{
	string searchPath = _directory + "*" + _extension;

	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				// white가 최상단이 되도록
				if(!strcmp(findData.cFileName, "white.png"))
					continue;

				m_vTextureList.push_back(_directory + findData.cFileName);
				m_vTextureNameList.push_back(findData.cFileName);
			}
		} while (FindNextFileA(hFind, &findData));

		FindClose(hFind);
	}
}
