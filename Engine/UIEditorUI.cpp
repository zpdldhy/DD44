#include "pch.h"
#include "UIEditorUI.h"
#include "PrefabLoader.h"
#include "AUIActor.h"
#include "UMaterial.h"
#include "Texture.h"
#include "Functions.h"
#include "Input.h"
#include "UIManager.h"
#include "CollisionManager.h"

void UIEditorUI::DrawUI()
{
	// ───────────────────────────── Init ─────────────────────────────	

	SelectActor();

	if (m_pUIActor)
	{
		m_vTextureList.emplace_back("../Resources/Texture/UI/white.png");
		m_vTextureNameList.emplace_back("white.png");

		string searchPath = "../Resources/Texture/UI/";

		// 만약 다른 형식의 파일도 필요하다면 여기에 추가
		SearchFile(searchPath, ".png", m_vTextureList, m_vTextureNameList);
		SearchFile(searchPath, ".jpg", m_vTextureList, m_vTextureNameList);
		SearchFile(searchPath, ".bmp", m_vTextureList, m_vTextureNameList);
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

	// ───────────────────────────── Delete ─────────────────────────────
	ImGui::SameLine(0, 212.5f);

	if (ImGui::Button("Delete UI"))
	{
		m_pUIActor->SetState(make_shared<IdleUIState>());
		m_pUIActor->SetStateType(UIStateType::ST_IDLE);
		UI->DeleteUI(m_pUIActor->m_Index);
		m_pUIActor = nullptr;
		ResetData();

		return;
	}

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

	m_ptCurrentMousePos = INPUT->GetMousePos();

	// 화면 좌표계로 변환
	m_ptCurrentMousePos.x = m_ptCurrentMousePos.x - g_windowSize.x / 2;
	m_ptCurrentMousePos.y = -(m_ptCurrentMousePos.y - g_windowSize.y / 2);

	// 마우스 위치로 이동
	if (m_pUIActor->GetStateType() == UIStateType::ST_ACTIVE)
	{

		auto scale = m_pUIActor->GetScale();
		auto pos = m_pUIActor->GetPosition();

		Vec2 vMin = { -0.4f, -0.4f };
		Vec2 vMax = { +0.4f, +0.4f };

		vMin = vMin * Vec2(scale) + Vec2(pos);
		vMax = vMax * Vec2(scale) + Vec2(pos);

		// Position 조정		
		if (Collision2D::CheckRectToPoint(m_ptCurrentMousePos, vMin, vMax))
		{
			m_Trans.Position[0] = static_cast<float>(m_ptCurrentMousePos.x);
			m_Trans.Position[1] = static_cast<float>(m_ptCurrentMousePos.y);
		}
		// Scale 조정
		else
		{
			POINT ptDiff = { m_ptCurrentMousePos.x - m_ptPrevMousePos.x, m_ptCurrentMousePos.y - m_ptPrevMousePos.y };

			// 아래 Scale 조정
			if (m_ptCurrentMousePos.y < vMin.y)
			{
				m_Trans.Scale[1] -= static_cast<float>(ptDiff.y);
				m_Trans.Position[1] += static_cast<float>(ptDiff.y) / 2;
			}

			// 위 Scale 조정
			if (m_ptCurrentMousePos.y > vMax.y)
			{
				m_Trans.Scale[1] += static_cast<float>(ptDiff.y);
				m_Trans.Position[1] += static_cast<float>(ptDiff.y) / 2;
			}

			// 왼쪽 Scale 조정
			if (m_ptCurrentMousePos.x < vMin.x)
			{
				m_Trans.Scale[0] -= static_cast<float>(ptDiff.x);
				m_Trans.Position[0] += static_cast<float>(ptDiff.x) / 2;
			}

			// 오른쪽 Scale 조정
			if (m_ptCurrentMousePos.x > vMax.x)
			{
				m_Trans.Scale[0] += static_cast<float>(ptDiff.x);
				m_Trans.Position[0] += static_cast<float>(ptDiff.x) / 2;
			}
		}
	}

	m_ptPrevMousePos = m_ptCurrentMousePos;

	// ───────────────────────────── Materials ─────────────────────────────

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Materials");

	SetTexture();

	ImGui::InputText("Shader", m_szShaderPath, IM_ARRAYSIZE(m_szShaderPath));

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	// ───────────────────────────── Update ─────────────────────────────

	UpdatePrefabData();
	UpdateUIActor();

	// ───────────────────────────── Prefab Save/Load/Delete ─────────────────────────────

	string searchPath = "../Resources/Prefab/";

	// 만약 다른 형식의 파일도 필요하다면 여기에 추가
	SearchFile(searchPath, ".ui.json", m_vPrefabList, m_vPrefabNameList);

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Prefab Tools");

	SavePrefab();

	ImGui::Separator(); ImGui::Spacing();

	LoadPrefab();

	ImGui::Separator(); ImGui::Spacing();

	DeletePrefab();

	m_vPrefabList.clear();
	m_vPrefabNameList.clear();
	m_vTextureList.clear();
	m_vTextureNameList.clear();
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

void UIEditorUI::UpdatePrefabData()
{
	m_CurrentPrefab.Name = to_wm(m_pUIActor->m_szName);

	memcpy(m_CurrentPrefab.transform.Scale, m_Trans.Scale, sizeof(float) * 3);
	memcpy(m_CurrentPrefab.transform.Rotation, m_Trans.Rotation, sizeof(float) * 3);
	memcpy(m_CurrentPrefab.transform.Position, m_Trans.Position, sizeof(float) * 3);

	// Texture
	m_CurrentPrefab.IdleTexturePath = m_vTextureList[m_iIdleIndex];
	m_CurrentPrefab.HoverTexturePath = m_vTextureList[m_iHoverIndex];
	m_CurrentPrefab.ActiveTexturePath = m_vTextureList[m_iActiveIndex];
	m_CurrentPrefab.SelectedTexturePath = m_vTextureList[m_iSelectedIndex];

	m_CurrentPrefab.ShaderPath = m_szShaderPath;
}

void UIEditorUI::UpdateUIActor()
{
	if (m_pUIActor == nullptr)
		return;

	m_pUIActor->SetPosition(Vec3(m_Trans.Position));
	m_pUIActor->SetRotation(Vec3(m_Trans.Rotation));
	m_pUIActor->SetScale(Vec3(m_Trans.Scale));

	m_pUIActor->SetPrefabData(m_CurrentPrefab);
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

	NamePtrs.clear();
}

void UIEditorUI::SavePrefab()
{
	ImGui::InputText("Prefab Name", m_szPrefabName, IM_ARRAYSIZE(m_szPrefabName));

	if (ImGui::Button("Save Prefab", ImVec2(-1, 0)))
	{
		PrefabLoader().SaveUI(m_CurrentPrefab, "../Resources/Prefab/" + std::string(m_szPrefabName) + ".ui.json");
	}
}

void UIEditorUI::LoadPrefab()
{
	static std::vector<const char*> NamePtrs;
	if (NamePtrs.empty())
	{
		for (auto& name : m_vPrefabNameList)
			NamePtrs.push_back(name.c_str());
	}

	ImGui::Combo("PrefabList", &m_iPrefabdIndex, NamePtrs.data(), (int)NamePtrs.size());

	if (ImGui::Button("Load Prefab##File", ImVec2(-1, 0)))
	{
		PrefabUIData data;
		std::string path = m_vPrefabList[m_iPrefabdIndex];

		if (PREFAB->LoadUI(path, data))
		{
			ResolvePrefabData(data);
		}
	}

	NamePtrs.clear();
}

void UIEditorUI::DeletePrefab()
{
	if (ImGui::Button("Delete Prefab##File", ImVec2(-1, 0)))
	{
		if (PREFAB->DeletePrefab(m_vPrefabList[m_iPrefabdIndex]))
		{
			m_iPrefabdIndex = 0;
		}
	}
}

void UIEditorUI::SearchFile(const string& _directory, const string& _extension, vector<string>& _vPaths, vector<string>& _vNames)
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
				if (!strcmp(findData.cFileName, "white.png"))
					continue;

				_vPaths.push_back(_directory + findData.cFileName);
				_vNames.push_back(findData.cFileName);
			}
		} while (FindNextFileA(hFind, &findData));

		FindClose(hFind);
	}
}

void UIEditorUI::SelectActor()
{
	// UI를 순회하며 Select를 찾아 Main Actor 변경
	for (auto pUI : UI->GetUIList())
	{
		if (pUI->GetStateType() == UIStateType::ST_SELECT)
		{
			m_pUIActor = pUI;
			m_CurrentPrefab = pUI->GetPrefabData();

			ResolvePrefabData(m_CurrentPrefab);
			break;
		}
	}
}

void UIEditorUI::ResolvePrefabData(const PrefabUIData& data)
{
	memcpy(m_Trans.Scale, data.transform.Scale, sizeof(float) * 3);
	memcpy(m_Trans.Rotation, data.transform.Rotation, sizeof(float) * 3);
	memcpy(m_Trans.Position, data.transform.Position, sizeof(float) * 3);

	// Texture
	for (size_t i = 0; i < m_vTextureList.size(); ++i)
	{
		if (!strcmp(m_vTextureList[i].c_str(), data.IdleTexturePath.c_str()))
			m_iIdleIndex = i;
		if (!strcmp(m_vTextureList[i].c_str(), data.HoverTexturePath.c_str()))
			m_iHoverIndex = i;
		if (!strcmp(m_vTextureList[i].c_str(), data.ActiveTexturePath.c_str()))
			m_iActiveIndex = i;
		if (!strcmp(m_vTextureList[i].c_str(), data.SelectedTexturePath.c_str()))
			m_iSelectedIndex = i;
	}

	strcpy_s(m_szShaderPath, data.ShaderPath.c_str());
}
