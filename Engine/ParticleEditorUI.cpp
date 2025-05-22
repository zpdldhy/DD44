#include "pch.h"
#include "ParticleEditorUI.h"
#include "PrefabLoader.h"
#include "AParticleActor.h"
#include "UMaterial.h"
#include "Texture.h"
#include "Functions.h"

void ParticleEditorUI::DrawUI()
{
	if (ImGui::Button("Create New Particle"))
	{
		if (!m_OnCreate)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: OnCreate callback is not set!");
			return;
		}

		// 새 파티클 생성
		auto newParticle = make_shared<AParticleActor>();
		m_pParticleActor = newParticle;

		ResetData();

		// 리스트에 추가
		m_vCreatedParticles.push_back(newParticle);
		m_iSelectedParticleIndex = static_cast<int>(m_vCreatedParticles.size()) - 1;

		// 콜백
		m_OnCreate(
			newParticle,
			m_vTextureList[0].c_str(),
			m_szShaderPath,
			m_Data,
			m_UVStart,
			m_UVEnd
		);
	}

	//ImGui::SameLine();
	ImGui::Text("Load Prefab:");
	ImGui::SameLine();

	static char loadName[64] = "MyParticle";
	ImGui::PushItemWidth(150);
	ImGui::InputText("##LoadName", loadName, IM_ARRAYSIZE(loadName));
	ImGui::PopItemWidth();

	ImGui::SameLine(); // 옆에 Load 버튼 붙이기
	if (ImGui::Button("Load"))
	{
		std::string path = "../Resources/Prefab/" + std::string(loadName) + ".particle.json";
		PrefabParticleData data;
		if (PREFAB->LoadParticle(data, path))
		{
			// 기존 로드 처리 (m_OnCreate 호출 포함)
			auto newParticle = make_shared<AParticleActor>();
			m_pParticleActor = newParticle;
			m_vCreatedParticles.push_back(newParticle);
			m_iSelectedParticleIndex = static_cast<int>(m_vCreatedParticles.size()) - 1;
			m_iLastSelectedParticleIndex = -1;

			m_Data.Position[0] = data.Translation.x;
			m_Data.Position[1] = data.Translation.y;
			m_Data.Position[2] = data.Translation.z;
			m_Data.Rotation[0] = data.Rotation.x;
			m_Data.Rotation[1] = data.Rotation.y;
			m_Data.Rotation[2] = data.Rotation.z;
			m_Data.Scale[0] = data.Scale.x;
			m_Data.Scale[1] = data.Scale.y;
			m_Data.Scale[2] = data.Scale.z;

			m_UVStart = data.UVStart;
			m_UVEnd = data.UVEnd;
			m_iDivisions = data.Divisions;
			m_iSelectedRow = data.Row;
			m_iSelectedCol = data.Col;
			strcpy_s(m_szShaderPath, data.ShaderPath.c_str());

			auto it = std::find(m_vTextureList.begin(), m_vTextureList.end(), data.TexturePath);
			if (it != m_vTextureList.end())
			{
				m_iSelectedTextureIndex = static_cast<int>(std::distance(m_vTextureList.begin(), it));
			}
			else
			{
				m_vTextureList.push_back(data.TexturePath);
				std::string texName = data.TexturePath.substr(data.TexturePath.find_last_of("/\\") + 1);
				m_vTextureNameList.push_back(texName);
				m_iSelectedTextureIndex = (int)m_vTextureList.size() - 1;
			}

			if (m_OnCreate)
			{
				m_OnCreate(
					newParticle,
					m_vTextureList[m_iSelectedTextureIndex].c_str(),
					m_szShaderPath,
					m_Data,
					m_UVStart,
					m_UVEnd
				);
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load prefab!");
		}
	}


	if (!m_pParticleActor)
		return;

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	ImGui::Text("Transform");
	DrawVec3("Position", m_Data.Position);
	DrawVec3("Rotation", m_Data.Rotation);
	DrawVec3("Scale", m_Data.Scale);

	ImGui::Spacing();
	ImGui::Text("UV");
	DrawVec2("UV Start", &m_UVStart.x);
	DrawVec2("UV End", &m_UVEnd.x);

	ImGui::Spacing();
	ImGui::Text("Sprite Grid");
	ImGui::InputInt("Divisions (NxN)", &m_iDivisions);
	ImGui::InputInt("Row", &m_iSelectedRow);
	ImGui::InputInt("Col", &m_iSelectedCol);
	if (ImGui::Button("Apply UV Grid"))
	{
		float cellSize = 1.0f / (float)m_iDivisions;
		m_UVStart.x = m_iSelectedCol * cellSize;
		m_UVStart.y = m_iSelectedRow * cellSize;
		m_UVEnd.x = (m_iSelectedCol + 1) * cellSize;
		m_UVEnd.y = (m_iSelectedRow + 1) * cellSize;
		if (m_pParticleActor)
		{
			m_pParticleActor->SetUV(m_UVStart, m_UVEnd);
		}
	}


	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	ImGui::Text("Assets");
	SetTexture();
	ImGui::InputText("Shader", m_szShaderPath, IM_ARRAYSIZE(m_szShaderPath));

	if (!m_vCreatedParticles.empty())
	{
		ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
		ImGui::Text("Created Particles");

		std::vector<const char*> particleNames;
		static std::vector<std::string> nameBuffers;

		nameBuffers.clear();
		for (int i = 0; i < m_vCreatedParticles.size(); ++i)
		{
			std::string name = "Particle " + std::to_string(i);
			nameBuffers.push_back(name);
			particleNames.push_back(nameBuffers.back().c_str());
		}

		ImGui::ListBox("##ParticleList", &m_iSelectedParticleIndex, particleNames.data(), (int)particleNames.size(), 5);

		if (m_iSelectedParticleIndex >= 0)
		{
			m_pParticleActor = m_vCreatedParticles[m_iSelectedParticleIndex];

			if (m_iSelectedParticleIndex != m_iLastSelectedParticleIndex)
			{
				// 선택이 바뀐 "그 프레임에만" 값을 복사
				Vec3 pos = m_pParticleActor->GetPosition();
				Vec3 rot = m_pParticleActor->GetRotation();
				Vec3 scale = m_pParticleActor->GetScale();

				m_Data.Position[0] = pos.x;
				m_Data.Position[1] = pos.y;
				m_Data.Position[2] = pos.z;

				m_Data.Rotation[0] = rot.x;
				m_Data.Rotation[1] = rot.y;
				m_Data.Rotation[2] = rot.z;

				m_Data.Scale[0] = scale.x;
				m_Data.Scale[1] = scale.y;
				m_Data.Scale[2] = scale.z;

				m_iLastSelectedParticleIndex = m_iSelectedParticleIndex; // 업데이트!
			}
		}
	}

	UpdateUIActor();
	// Prefab 저장 UI
	static char prefabName[64] = "MyParticle";
	ImGui::InputText("Prefab Name", prefabName, IM_ARRAYSIZE(prefabName));

	if (ImGui::Button("Save as Prefab"))
	{
		PrefabParticleData data;
		data.Name = prefabName;
		data.ShaderPath = m_szShaderPath;
		data.TexturePath = m_vTextureList[m_iSelectedTextureIndex];
		data.Scale = Vec3(m_Data.Scale);
		data.Rotation = Vec3(m_Data.Rotation);
		data.Translation = Vec3(m_Data.Position);
		data.Divisions = m_iDivisions;
		data.Row = m_iSelectedRow;
		data.Col = m_iSelectedCol;
		data.UVStart = m_UVStart;
		data.UVEnd = m_UVEnd;
		data.BillboardSizeX = m_Data.Scale[0]; // 또는 따로 관리하는 값
		data.BillboardSizeY = m_Data.Scale[1];

		std::string path = "../Resources/Prefab/" + data.Name + ".particle.json";
		PREFAB->SaveParticle(data, path);
	}

	ImGui::Separator();
}

void ParticleEditorUI::DrawVec3(const char* label, float* values)
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

void ParticleEditorUI::DrawVec2(const char* label, float* values)
{
	ImGui::PushID(label);
	ImGui::Text("%s", label);
	ImGui::Indent(10.0f);

	const float inputWidth = 60.0f;
	const float spacing = 20.0f;

	for (int i = 0; i < 2; ++i)
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


void ParticleEditorUI::UpdateUIActor()
{
	if (!m_pParticleActor)
		return;

	m_pParticleActor->SetPosition(Vec3(m_Data.Position));
	m_pParticleActor->SetRotation(Vec3(m_Data.Rotation));
	m_pParticleActor->SetScale(Vec3(m_Data.Scale));
}

void ParticleEditorUI::ResetData()
{

	for (int i = 0; i < 3; ++i)
	{
		m_Data.Position[i] = m_Data.Position[i];
		m_Data.Rotation[i] = 0.0f;
	}
	m_Data.Scale[0] = 20.0f;
	m_Data.Scale[1] = 20.0f;
	m_Data.Scale[2] = 1.0f;

	m_UVStart = { 0.0f, 0.0f };
	m_UVEnd = { 1.0f, 1.0f };

	m_iSelectedTextureIndex = 0;
	m_vTextureList.emplace_back("../Resources/Texture/white.png");
	m_vTextureNameList.emplace_back("white.png");

	m_iDivisions = 4;
	m_iSelectedRow = 0;
	m_iSelectedCol = 0;

	strcpy_s(m_szShaderPath, "../Resources/Shader/SpriteUV.hlsl");
}

void ParticleEditorUI::SetTexture()
{
	if (m_vTextureList.size() == 1)
	{
		string searchPath = "../Resources/Sprite/";
		SearchFile(searchPath, ".png");
		SearchFile(searchPath, ".jpg");
	}

	static std::vector<const char*> namePtrs;
	namePtrs.clear();
	if (namePtrs.empty())
	{
		for (auto& name : m_vTextureNameList)
			namePtrs.push_back(name.c_str());
	}

	static int preSelectedIndex = 0;
	ImGui::Combo("Texture", &m_iSelectedTextureIndex, namePtrs.data(), (int)namePtrs.size());

	if (m_iSelectedTextureIndex != preSelectedIndex)
	{
		preSelectedIndex = m_iSelectedTextureIndex;
		m_pParticleActor->GetMeshComponent()->GetMaterial()->SetTexture(
			TEXTURE->Get(to_mw(m_vTextureList[m_iSelectedTextureIndex]))
		);
	}
}

vector<string> ParticleEditorUI::GetTextureList(const std::string& directory)
{
	vector<string> out;
	for (const auto& path : m_vTextureList)
	{
		if (path.find(directory) != string::npos)
			out.push_back(path);
	}
	return out;
}

void ParticleEditorUI::SearchFile(const string& directory, const string& extension)
{
	string searchPath = directory + "*" + extension;
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (!strcmp(findData.cFileName, "white.png"))
					continue;

				m_vTextureList.push_back(directory + findData.cFileName);
				m_vTextureNameList.push_back(findData.cFileName);
			}
		} while (FindNextFileA(hFind, &findData));

		FindClose(hFind);
	}
}