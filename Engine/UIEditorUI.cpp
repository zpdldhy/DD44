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
	// ���������������������������������������������������������� Init ����������������������������������������������������������	

	m_vTextureList.emplace_back("../Resources/Texture/UI/white.png");
	m_vTextureNameList.emplace_back("white.png");

	string searchPath = "../Resources/Texture/UI/";

	// ���� �ٸ� ������ ���ϵ� �ʿ��ϴٸ� ���⿡ �߰�
	SearchFile(searchPath, ".png", m_vTextureList, m_vTextureNameList);
	SearchFile(searchPath, ".jpg", m_vTextureList, m_vTextureNameList);
	SearchFile(searchPath, ".bmp", m_vTextureList, m_vTextureNameList);


	SelectActor();

	}


	// ���������������������������������������������������������� Create ����������������������������������������������������������
	if (ImGui::Button("Create New UI"))
	{
		m_pUIActor = make_shared<AUIActor>();

		ResetData();

		// �ʱⰪ ����
		m_OnCreate(
			m_pUIActor,
			m_Trans,
			Vec4(m_vSliceUV)
			m_szShaderPath,
			m_Trans
		);
	}

	if (m_pUIActor == nullptr)
	{
		m_vPrefabList.clear();
		m_vPrefabNameList.clear();
		m_vTextureList.clear();
		m_vTextureNameList.clear();
		return;
	// ���������������������������������������������������������� Delete ����������������������������������������������������������
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

	}



	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	// ���������������������������������������������������������� Transform ����������������������������������������������������������
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");

	DrawVec3("Scale   ", m_Trans.Scale);
	SetTexture();
	// ���������������������������������������������������������� SliceUV ����������������������������������������������������������
	if (m_pUIActor->GetStateType() == UIStateType::ST_ACTIVE)
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "SliceUV");

	SetSliceUV();

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	// ���������������������������������������������������������� Materials ����������������������������������������������������������

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Materials");

	SetTexture();

	ImGui::InputText("Shader", m_szShaderPath, IM_ARRAYSIZE(m_szShaderPath));

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	// ���������������������������������������������������������� Update ����������������������������������������������������������

	UpdatePrefabData();
	UpdateUIActor();

	// ���������������������������������������������������������� Prefab Save/Load/Delete ����������������������������������������������������������

	searchPath = "../Resources/Prefab/";

	// ���� �ٸ� ������ ���ϵ� �ʿ��ϴٸ� ���⿡ �߰�
	SearchFile(searchPath, ".ui.json", m_vPrefabList, m_vPrefabNameList);

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Prefab Tools");

	SavePrefab();

	ImGui::Separator(); ImGui::Spacing();

	LoadPrefab();

			// ���� Scale ����

	DeletePrefab();

	m_vPrefabList.clear();
	m_vPrefabNameList.clear();
	m_vTextureList.clear();
	m_vTextureNameList.clear();
			if (m_ptCurrentMousePos.x < vMin.x)
			{
				m_Trans.Scale[0] -= static_cast<float>(ptDiff.x);
				m_Trans.Position[0] += static_cast<float>(ptDiff.x) / 2;
			}

			// ������ Scale ����
			if (m_ptCurrentMousePos.x > vMax.x)
			{
				m_Trans.Scale[0] += static_cast<float>(ptDiff.x);
				m_Trans.Position[0] += static_cast<float>(ptDiff.x) / 2;
			}
		}
	}

	m_ptPrevMousePos = m_ptCurrentMousePos;

	// ���������������������������������������������������������� Materials ����������������������������������������������������������

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Materials");
		ImGui::PopItemWidth();
	SetTexture();

	ImGui::InputText("Shader", m_szShaderPath, IM_ARRAYSIZE(m_szShaderPath));

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	// ���������������������������������������������������������� Update ����������������������������������������������������������

void UIEditorUI::UpdatePrefabData()
{
	m_CurrentPrefab.Name = to_wm(m_pUIActor->m_szName);

	memcpy(m_CurrentPrefab.transform.Scale, m_Trans.Scale, sizeof(float) * 3);
	memcpy(m_CurrentPrefab.transform.Rotation, m_Trans.Rotation, sizeof(float) * 3);
	memcpy(m_CurrentPrefab.transform.Position, m_Trans.Position, sizeof(float) * 3);

	memcpy(m_CurrentPrefab.SliceUV, m_vSliceUV, sizeof(float) * 4);

	// Texture
	m_CurrentPrefab.IdleTexturePath = m_vTextureList[m_iIdleIndex];
	m_CurrentPrefab.HoverTexturePath = m_vTextureList[m_iHoverIndex];
	m_CurrentPrefab.ActiveTexturePath = m_vTextureList[m_iActiveIndex];
	m_CurrentPrefab.SelectedTexturePath = m_vTextureList[m_iSelectedIndex];

	m_CurrentPrefab.ShaderPath = m_szShaderPath;
}


	ImGui::Separator(); ImGui::Spacing();
	if (m_pUIActor == nullptr)

void UIEditorUI::DrawVec3(const char* label, float* values)
	m_pUIActor->SetPosition(Vec3(m_Trans.Position));
	m_pUIActor->SetRotation(Vec3(m_Trans.Rotation));
	m_pUIActor->SetScale(Vec3(m_Trans.Scale));

	m_pUIActor->SetSliceData(Vec4(m_vSliceUV[0], m_vSliceUV[1], m_vSliceUV[2], m_vSliceUV[3]));

	m_pUIActor->SetPrefabData(m_CurrentPrefab);
	const float inputWidth = 60.0f;
	const float spacing = 20.0f;


	for (int i = 0; i < 3; ++i)
	for (int i = 0; i < 3; i++) m_Trans.Position[i] = 0.0f;
	for (int i = 0; i < 3; i++) m_Trans.Rotation[i] = 0.0f;
	for (int i = 0; i < 2; i++) m_Trans.Scale[i] = 100.0f;
	// Slice
	for (int i = 0; i < 4; i++) m_vSliceUV[i] = 0.5f;

	m_Trans.Scale[2] = 1.0f;

		ImGui::PushItemWidth(inputWidth);
	m_iIdleIndex = 0;
	m_iHoverIndex = 0;
	m_iActiveIndex = 0;
	m_iSelectedIndex = 0;
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
	if(m_pUIActor==nullptr)
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
	m_iSelectedTextureIndex = 0;
	m_vTextureList.emplace_back("../Resources/Texture/white.png");
	m_vTextureNameList.emplace_back("white.png");

	strcpy_s(m_szShaderPath, "../Resources/Shader/UVSlice.hlsl");
}

void UIEditorUI::SetTransform()
{
	ImGui::SameLine(0, 37.5f); ImGui::Text("%s", "X");
	ImGui::SameLine(0, 72.5f); ImGui::Text("%s", "Y");
	ImGui::SameLine(0, 72.5f); ImGui::Text("%s", "Z");

	DrawVec3("Position", m_Trans.Position);
	DrawVec3("Rotation", m_Trans.Rotation);
	DrawVec3("Scale   ", m_Trans.Scale);

	m_ptCurrentMousePos = INPUT->GetMousePos();

	// ȭ�� ��ǥ��� ��ȯ
	m_ptCurrentMousePos.x = m_ptCurrentMousePos.x - g_windowSize.x / 2;
	m_ptCurrentMousePos.y = -(m_ptCurrentMousePos.y - g_windowSize.y / 2);

	// ���콺 ��ġ�� �̵�
	if (m_pUIActor->GetStateType() == UIStateType::ST_ACTIVE)
	{

		auto scale = m_pUIActor->GetScale();
		auto pos = m_pUIActor->GetPosition();

		Vec2 vMin = { -0.4f, -0.4f };
		Vec2 vMax = { +0.4f, +0.4f };

		vMin = vMin * Vec2(scale) + Vec2(pos);
		vMax = vMax * Vec2(scale) + Vec2(pos);

		// Position ����		
		if (Collision2D::CheckRectToPoint(m_ptCurrentMousePos, vMin, vMax))
		{
			m_Trans.Position[0] = static_cast<float>(m_ptCurrentMousePos.x);
			m_Trans.Position[1] = static_cast<float>(m_ptCurrentMousePos.y);
		}
		// Scale ����
		else
		{
			POINT ptDiff = { m_ptCurrentMousePos.x - m_ptPrevMousePos.x, m_ptCurrentMousePos.y - m_ptPrevMousePos.y };

			// �Ʒ� Scale ����
			if (m_ptCurrentMousePos.y < vMin.y)
			{
				m_Trans.Scale[1] -= static_cast<float>(ptDiff.y);
				m_Trans.Position[1] += static_cast<float>(ptDiff.y) / 2;
			}

			// �� Scale ����
			if (m_ptCurrentMousePos.y > vMax.y)
			{
				m_Trans.Scale[1] += static_cast<float>(ptDiff.y);
				m_Trans.Position[1] += static_cast<float>(ptDiff.y) / 2;
			}

			// ���� Scale ����
			if (m_ptCurrentMousePos.x < vMin.x)
	static std::vector<const char*> NamePtrs;
	if (NamePtrs.empty())
				m_Trans.Scale[0] -= static_cast<float>(ptDiff.x);
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
			}
void UIEditorUI::SavePrefab()
{
	ImGui::InputText("Prefab Name", m_szPrefabName, IM_ARRAYSIZE(m_szPrefabName));

	if (ImGui::Button("Save Prefab", ImVec2(-1, 0)))
	{
		PrefabLoader().SaveUI(m_CurrentPrefab, "../Resources/Prefab/" + std::string(m_szPrefabName) + ".ui.json");
			{
}
				m_Trans.Scale[0] += static_cast<float>(ptDiff.x);
void UIEditorUI::LoadPrefab()
{
				m_Trans.Position[0] += static_cast<float>(ptDiff.x) / 2;
			}
		}
		for (auto& name : m_vPrefabNameList)

	m_ptPrevMousePos = m_ptCurrentMousePos;
}
	ImGui::Combo("PrefabList", &m_iPrefabdIndex, NamePtrs.data(), (int)NamePtrs.size());
void UIEditorUI::SetSliceUV()
	if (ImGui::Button("Load Prefab##File", ImVec2(-1, 0)))
	{
		PrefabUIData data;
		std::string path = m_vPrefabList[m_iPrefabdIndex];
	ImGui::SameLine(0, 25.f); ImGui::Text("%s", "Left");
		if (PREFAB->LoadUI(path, data))
		{
			ResolvePrefabData(data);
		}
	}
	ImGui::Indent(60.f);
	NamePtrs.clear();

	const float inputWidth = 60.0f;
void UIEditorUI::DeletePrefab()

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
	for (int i = 0; i < 4; ++i)
	{
		ImGui::PushID(i);

		ImGui::PushItemWidth(inputWidth);
		ImGui::InputFloat("##Value", &m_vSliceUV[i], 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopID();
		ImGui::SameLine(0, spacing);
	}

				if (!strcmp(findData.cFileName, "white.png"))
	ImGui::PopID();
}
				_vPaths.push_back(_directory + findData.cFileName);
				_vNames.push_back(findData.cFileName);
{
	if (m_vTextureList.size() == 1)
	{
		string searchPath = "../Resources/Texture/";

}

void UIEditorUI::SelectActor()
{
	// UI�� ��ȸ�ϸ� Select�� ã�� Main Actor ����
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

	memcpy(m_vSliceUV, data.SliceUV, sizeof(float) * 4);

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
				if(!strcmp(findData.cFileName, "white.png"))
					continue;

				m_vTextureList.push_back(_directory + findData.cFileName);
				m_vTextureNameList.push_back(findData.cFileName);
			}
		} while (FindNextFileA(hFind, &findData));

		FindClose(hFind);
	}
}

void UIEditorUI::SelectActor()
{
	// UI�� ��ȸ�ϸ� Select�� ã�� Main Actor ����
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
