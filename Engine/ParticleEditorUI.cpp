#include "pch.h"
#include "ParticleEditorUI.h"
#include "PrefabLoader.h"
#include "AParticleActor.h"
#include "UMaterial.h"
#include "Texture.h"
#include "Functions.h"


void ParticleEditorUI::DrawUI()
{
	static bool bInit = false;
	if (!bInit)
	{
		bInit = true;
		ResetData();
	}

	if (ImGui::Button("Create New Particle"))
	{
		if (!m_OnCreate)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: OnCreate callback is not set!");
			return;
		}
		auto newParticle = make_shared<AParticleActor>();

		m_vCreatedParticles.push_back(newParticle);
		m_vParticleData.push_back(m_Data);
		m_iSelectedParticleIndex = static_cast<int>(m_vCreatedParticles.size()) - 1;
		
		ActorData copy;
		for (int j = 0; j < 3; ++j)
		{
			copy.Position[j] = m_Data.Position[j];
			copy.Rotation[j] = m_Data.Rotation[j];
			copy.Scale[j] = m_Data.Scale[j];
		}
		// 콜백
		m_OnCreate(
			newParticle,
			m_vTextureList[0].c_str(),
			m_szShaderPath,
			m_Data,
			m_UVStart,
			m_UVEnd,
			m_iDivisions,
			m_AnimData.DurationSeconds,
			m_AnimData.bLoop,
			m_AnimData.bAutoDestroy
		);

		newParticle->InitSpriteAnimation(m_iDivisions, m_AnimData.DurationSeconds);
		newParticle->SetLoop(m_AnimData.bLoop);
		newParticle->SetAutoDestroy(m_AnimData.bAutoDestroy);
		newParticle->SetUV(m_UVStart, m_UVEnd);
	}

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
			auto newParticle = make_shared<AParticleActor>();
			m_vCreatedParticles.push_back(newParticle);
			m_vParticleData.push_back(m_Data);

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
					m_vTextureList[0].c_str(),
					m_szShaderPath,
					m_Data,
					m_UVStart,
					m_UVEnd,
					m_iDivisions,
					m_AnimData.DurationSeconds,
					m_AnimData.bLoop,
					m_AnimData.bAutoDestroy
				);
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load prefab!");
		}
	}

	ImGui::Text("Load Group:");
	ImGui::SameLine();
	static char loadGroupName[64] = "MyParticleGroup";
	ImGui::PushItemWidth(150);
	ImGui::InputText("##LoadGroupName", loadGroupName, IM_ARRAYSIZE(loadGroupName));
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Load Group"))
	{
		std::string path = "../Resources/Prefab/" + std::string(loadGroupName) + ".particlegroup.json";

		PrefabParticleGroupData group;
		if (!PREFAB->LoadParticleGroup(group, path))
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load group!");
		}
		else
		{
			for (auto& p : group.Particles)
			{
				auto newParticle = make_shared<AParticleActor>();
				m_vCreatedParticles.push_back(newParticle);
				ActorData tempData;
				tempData.Position[0] = p.Translation.x;
				tempData.Position[1] = p.Translation.y;
				tempData.Position[2] = p.Translation.z;

				tempData.Rotation[0] = p.Rotation.x;
				tempData.Rotation[1] = p.Rotation.y;
				tempData.Rotation[2] = p.Rotation.z;

				tempData.Scale[0] = p.Scale.x;
				tempData.Scale[1] = p.Scale.y;
				tempData.Scale[2] = p.Scale.z;

				
				m_vParticleData.push_back(tempData);

				if (m_OnCreate)
				{
					m_OnCreate(
						newParticle,
						p.TexturePath.c_str(),
						p.ShaderPath.c_str(),
						tempData,
						p.UVStart,
						p.UVEnd,
						p.Divisions,
						p.Duration,
						p.bLoop,
						p.bAutoDestroy
					);
				}		
			}

			ImGui::TextColored(ImVec4(0, 1, 0, 1), "Group loaded!");
		}

	}

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();


	ImGui::Text("Transform");
	DrawVec3("Position", m_Data.Position);
	DrawVec3("Rotation", m_Data.Rotation);
	DrawVec3("Scale", m_Data.Scale);

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	ImGui::Text("Sprite Grid");
	ImGui::InputInt("Divisions (NxN)", &m_iDivisions);
	if (ImGui::Button("Apply UV Grid"))
	{
		float cellSize = 1.0f / (float)m_iDivisions;
		m_UVStart.x = m_iSelectedCol * cellSize;
		m_UVStart.y = m_iSelectedRow * cellSize;
		m_UVEnd.x = (m_iSelectedCol + 1) * cellSize;
		m_UVEnd.y = (m_iSelectedRow + 1) * cellSize;
		if (auto actor = GetSelectedActor())
		{
			actor->SetUV(m_UVStart, m_UVEnd);

			float duration = actor->GetDuration();
			actor->InitSpriteAnimation(m_iDivisions, duration);
		}
	}
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("Animation Settings");
	static float prevDuration = m_AnimData.DurationSeconds;

	if (ImGui::InputFloat("Duration (sec)", &m_AnimData.DurationSeconds))
	{
		if (auto actor = GetSelectedActor())
			actor->SetDuration(m_AnimData.DurationSeconds);
	}
	if (ImGui::Checkbox("Loop", &m_AnimData.bLoop))
	{
		if (auto actor = GetSelectedActor())
			actor->SetLoop(m_AnimData.bLoop);
	}

	if (ImGui::Checkbox("Auto Destroy", &m_AnimData.bAutoDestroy))
	{
		if (auto actor = GetSelectedActor())
			actor->SetAutoDestroy(m_AnimData.bAutoDestroy);
	}
	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	ImGui::Text("Batch Particle Spawn");

	DrawVec3("Box Min (XYZ)", m_vBoxMin);
	DrawVec3("Box Max (XYZ)", m_vBoxMax);
	ImGui::InputInt("Count", &m_iNumParticles);

	if (ImGui::Button("Spawn Particles in Box"))
	{
		for (int i = 0; i < m_iNumParticles; ++i)
		{
			Vec3 randPos = RandomVec3InBox(m_vBoxMin, m_vBoxMax);

			ActorData copy = m_Data;
			copy.Position[0] = randPos.x;
			copy.Position[1] = randPos.y;
			copy.Position[2] = randPos.z;

			auto newParticle = make_shared<AParticleActor>();
			m_vCreatedParticles.push_back(newParticle);
			m_vParticleData.push_back(copy);

			if (m_OnCreate)
			{
				m_OnCreate(
					newParticle,
					m_vTextureList[m_iSelectedTextureIndex].c_str(),
					m_szShaderPath,
					copy,
					m_UVStart,
					m_UVEnd,
					m_iDivisions,
					m_AnimData.DurationSeconds,
					m_AnimData.bLoop,
					m_AnimData.bAutoDestroy
				);
			}

			newParticle->InitSpriteAnimation(m_iDivisions, m_AnimData.DurationSeconds);
			newParticle->SetLoop(m_AnimData.bLoop);
			newParticle->SetAutoDestroy(m_AnimData.bAutoDestroy);
			newParticle->SetUV(m_UVStart, m_UVEnd);
			newParticle->SetPosition(randPos);
			newParticle->SetRotation(Vec3(copy.Rotation));
			newParticle->SetScale(Vec3(copy.Scale));
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

		if (m_iSelectedParticleIndex >= 0 && m_iSelectedParticleIndex < m_vParticleData.size())
		{
			m_Data = m_vParticleData[m_iSelectedParticleIndex];
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
		data.Duration = m_AnimData.DurationSeconds;
		data.bLoop = m_AnimData.bLoop;
		data.bAutoDestroy = m_AnimData.bAutoDestroy;

		std::string path = "../Resources/Prefab/" + data.Name + ".particle.json";
		PREFAB->SaveParticle(data, path);
	}

	ImGui::Separator();
	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	ImGui::Text("Prefab Group");

	ImGui::InputText("Group Name", m_szGroupName, IM_ARRAYSIZE(m_szGroupName));

	if (ImGui::Button("Save All as Group"))
	{
		PrefabParticleGroupData group;
		group.GroupName = m_szGroupName;

		for (int i = 0; i < m_vParticleData.size(); ++i)
		{
			PrefabParticleData data;
			data.Name = "Particle" + std::to_string(i); // 이름 부여

			data.ShaderPath = m_szShaderPath;
			data.TexturePath = m_vTextureList[m_iSelectedTextureIndex];

			data.Scale = Vec3(m_vParticleData[i].Scale);
			data.Rotation = Vec3(m_vParticleData[i].Rotation);
			data.Translation = Vec3(m_vParticleData[i].Position);

			data.Divisions = m_iDivisions;
			data.Row = m_iSelectedRow;
			data.Col = m_iSelectedCol;
			data.UVStart = m_UVStart;
			data.UVEnd = m_UVEnd;

			data.Duration = m_AnimData.DurationSeconds;
			data.bLoop = m_AnimData.bLoop;
			data.bAutoDestroy = m_AnimData.bAutoDestroy;

			group.Particles.push_back(data);
		}

		std::string path = "../Resources/Prefab/";
		path += std::string(m_szGroupName) + ".particlegroup.json";

		if (!PREFAB->SaveParticleGroup(group, path))
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to save group!");
		else
			ImGui::TextColored(ImVec4(0, 1, 0, 1), "Group saved!");
	}
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

void ParticleEditorUI::DrawVec3(const char* label, Vec3& vec)
{
	DrawVec3(label, &vec.x);
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
	if (m_iSelectedParticleIndex >= m_vParticleData.size()) 
		return;

	auto actor = m_vCreatedParticles[m_iSelectedParticleIndex];
	auto& data = m_vParticleData[m_iSelectedParticleIndex];


	actor->SetPosition(Vec3(m_Data.Position));
	actor->SetRotation(Vec3(m_Data.Rotation));
	actor->SetScale(Vec3(m_Data.Scale));

	data = m_Data;
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

	if (m_vTextureList.empty())
	{
		m_vTextureList.emplace_back("../Resources/Texture/white.png");
		m_vTextureNameList.emplace_back("white.png");
	}

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
	for (auto& name : m_vTextureNameList)
		namePtrs.push_back(name.c_str());
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
		auto selectedActor = GetSelectedActor();
		if (selectedActor)
		{
			selectedActor->GetMeshComponent()->GetMaterial()->SetTexture(
				TEXTURE->Get(to_mw(m_vTextureList[m_iSelectedTextureIndex]))
			);
		}
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

Vec3 ParticleEditorUI::RandomVec3InBox(const Vec3& min, const Vec3& max)
{
	return Vec3(
		RandomRange(min.x, max.x),
		RandomRange(min.y, max.y),
		RandomRange(min.z, max.z)
	);
}


shared_ptr<AParticleActor> ParticleEditorUI::GetSelectedActor()
{
	if (m_iSelectedParticleIndex < 0 || m_iSelectedParticleIndex >= m_vCreatedParticles.size())
		return nullptr;
	return m_vCreatedParticles[m_iSelectedParticleIndex];
}


