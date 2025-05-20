#include "pch.h"
#include "UIEditorUI.h"
#include "PrefabLoader.h"
#include "AUIActor.h"
#include "UMaterial.h"
#include "Texture.h"
#include "Functions.h"

void UIEditorUI::DrawUI()
{
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
			m_UIData
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

	DrawVec3("Position", m_UIData.Position);
	DrawVec3("Rotation", m_UIData.Rotation);
	DrawVec3("Scale   ", m_UIData.Scale);

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	// ───────────────────────────── Asset Input ─────────────────────────────
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Asset Paths");
	SetTexture();
	ImGui::InputText("Shader", m_szShaderPath, IM_ARRAYSIZE(m_szShaderPath));

	// ───────────────────────────── Actor Update ─────────────────────────────
	UpdateUIActor();

	ImGui::Spacing();
	// ───────────────────────────── Prefab Save/Load ─────────────────────────────
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Prefab Tools");

	static char prefabName[64] = "MyUI";
	ImGui::InputText("Prefab Name", prefabName, IM_ARRAYSIZE(prefabName));

	if (ImGui::Button("Save as Prefab", ImVec2(-1, 0)))
	{
		PrefabObjectData data;
		data.Name = prefabName;
		data.ShaderPath = m_szShaderPath;
		//data.TexturePath = m_szTexturePath;
		data.Scale = Vec3(m_UIData.Scale);
		data.Rotation = Vec3(m_UIData.Rotation);
		data.Translation = Vec3(m_UIData.Position);

		PrefabLoader().SaveObject(data, "../Resources/Prefab/" + data.Name + ".ui.json");
	}

	if (ImGui::Button("Load Prefab##File", ImVec2(-1, 0)))
	{
		PrefabObjectData data;
		std::string path = "../Resources/Prefab/" + std::string(prefabName) + ".ui.json";
		if (PREFAB->LoadObject(path, data))
		{
			strcpy_s(m_szShaderPath, data.ShaderPath.c_str());
			//strcpy_s(m_szTexturePath, data.TexturePath.c_str());

			
		}
	}

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

	m_pUIActor->SetPosition(Vec3(m_UIData.Position));
	m_pUIActor->SetRotation(Vec3(m_UIData.Rotation));
	m_pUIActor->SetScale(Vec3(m_UIData.Scale));
}

void UIEditorUI::ResetData()
{
	// Transform
	for (int i = 0; i < 3; i++) m_UIData.Position[i] = 0.0f;
	for (int i = 0; i < 3; i++) m_UIData.Rotation[i] = 0.0f;
	for (int i = 0; i < 2; i++) m_UIData.Scale[i] = 100.0f;
	m_UIData.Scale[2] = 1.0f;

	// Texture
	m_iSelectedTextureIndex = 0;
	m_vTextureList.emplace_back("../Resources/Texture/white.png");
	m_vTextureNameList.emplace_back("white.png");

	strcpy_s(m_szShaderPath, "../Resources/Shader/Default.hlsl");
}

void UIEditorUI::SetTexture()
{
	if (m_vTextureList.size() == 1)
	{
		string searchPath = "../Resources/Texture/";

		// 만약 다른 형식의 파일도 필요하다면 여기에 추가
		SearchFile(searchPath, ".png");
		SearchFile(searchPath, ".jpg");
	}

	static std::vector<const char*> NamePtrs;
	if (NamePtrs.empty())
	{
		for (auto& name : m_vTextureNameList)
			NamePtrs.push_back(name.c_str());
	}

	static int iPreSelectedIndex = 0;

	ImGui::Combo("Texture", &m_iSelectedTextureIndex, NamePtrs.data(), (int)NamePtrs.size());

	if (m_iSelectedTextureIndex != iPreSelectedIndex)
	{
		iPreSelectedIndex = m_iSelectedTextureIndex;
		m_pUIActor->GetMeshComponent()->GetMaterial()->SetTexture(TEXTURE->Get(to_mw(m_vTextureList[iPreSelectedIndex])));
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
