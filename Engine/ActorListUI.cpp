#include "pch.h"
#include "ActorListUI.h"
#include "ObjectManager.h"
#include "AActor.h"
#include "ATerrainTileActor.h"
#include "USkinnedMeshComponent.h"
#include "UMaterial.h"
#include "PrefabLoader.h"

void ActorListUI::DrawUI()
{
	if (!m_bVisible) return;

	ImGui::SetNextWindowPos(ImVec2(10.0f, ImGui::GetIO().DisplaySize.y - 260.0f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);

	ImGui::Begin("Actor List", &m_bVisible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	auto& actorMap = OBJECT->GetActorList();

	for (auto it = actorMap.begin(); it != actorMap.end(); ++it)
	{
		UINT id = it->first;
		auto actor = it->second;

		std::wstring wName = actor->m_szName;
		std::string name(wName.begin(), wName.end());

		std::string label = "Actor " + std::to_string(id);
		if (!name.empty())
			label += " - " + name;

		bool selected = (m_iSelectedActorID == id) ||
			(std::find(m_vMultiSelectedIDs.begin(), m_vMultiSelectedIDs.end(), id) != m_vMultiSelectedIDs.end());

		if (ImGui::Selectable(label.c_str(), selected))
		{
			bool bShift = ImGui::GetIO().KeyShift;

			if (bShift)
			{
				m_bMultiSelectMode = true;

				auto found = std::find(m_vMultiSelectedIDs.begin(), m_vMultiSelectedIDs.end(), id);
				if (found != m_vMultiSelectedIDs.end())
					m_vMultiSelectedIDs.erase(found);
				else
					m_vMultiSelectedIDs.push_back(id);

				m_iSelectedActorID = id;
			}
			else
			{
				m_bMultiSelectMode = false;

				for (UINT oldID : m_vMultiSelectedIDs)
				{
					if (oldID == id) continue;
					auto itOff = actorMap.find(oldID);
					if (itOff != actorMap.end())
					{
						auto actorOff = itOff->second;
						auto mesh = actorOff->GetMeshComponent<UMeshComponent>();
						if (mesh && mesh->GetMaterial())
							mesh->GetMaterial()->SetGlowParams(0.f, Vec3(0.f, 0.f, 0.f));
					}
				}

				m_vMultiSelectedIDs.clear();
				m_vMultiSelectedIDs.push_back(id);
				m_iSelectedActorID = id;
			}
		}
	}

	ImGui::Separator(); ImGui::Spacing();

	if (!m_bMultiSelectMode)
	{
		for (UINT id : m_vLastMultiSelectedIDs)
		{
			if (id == m_iSelectedActorID) continue;

			auto it = actorMap.find(id);
			if (it != actorMap.end())
			{
				auto actor = it->second;
				auto mesh = actor->GetMeshComponent<UMeshComponent>();
				if (mesh && mesh->GetMaterial())
					mesh->GetMaterial()->SetGlowParams(0.f, Vec3(0.f, 0.f, 0.f));
			}
		}

		if (m_iSelectedActorID != m_iLastActorID)
		{
			if (m_iLastActorID >= 0)
			{
				auto it = actorMap.find(m_iLastActorID);
				if (it != actorMap.end())
				{
					auto actor = it->second;
					auto mesh = actor->GetMeshComponent<UMeshComponent>();
					if (mesh && mesh->GetMaterial())
						mesh->GetMaterial()->SetGlowParams(0.f, Vec3(0.f, 0.f, 0.f));
				}
			}

			auto it = actorMap.find(m_iSelectedActorID);
			if (it != actorMap.end())
			{
				auto actor = it->second;
				auto mesh = actor->GetMeshComponent<UMeshComponent>();
				if (mesh && mesh->GetMaterial())
					mesh->GetMaterial()->SetGlowParams(0.7f, Vec3(1.0f, 0.0f, 0.0f));
			}

			m_iLastActorID = m_iSelectedActorID;
		}
	}
	else
	{
		for (UINT oldID : m_vLastMultiSelectedIDs)
		{
			if (std::find(m_vMultiSelectedIDs.begin(), m_vMultiSelectedIDs.end(), oldID) == m_vMultiSelectedIDs.end())
			{
				auto it = actorMap.find(oldID);
				if (it != actorMap.end())
				{
					auto actor = it->second;
					auto mesh = actor->GetMeshComponent<UMeshComponent>();
					if (mesh && mesh->GetMaterial())
						mesh->GetMaterial()->SetGlowParams(0.f, Vec3(0.f, 0.f, 0.f));
				}
			}
		}

		for (UINT id : m_vMultiSelectedIDs)
		{
			auto it = actorMap.find(id);
			if (it != actorMap.end())
			{
				auto actor = it->second;
				auto mesh = actor->GetMeshComponent<UMeshComponent>();
				if (mesh && mesh->GetMaterial())
					mesh->GetMaterial()->SetGlowParams(0.7f, Vec3(1.0f, 0.0f, 0.0f));
			}
		}
	}

	ImGui::End();

	if (m_iSelectedActorID >= 0)
	{
		ImGui::SetNextWindowPos(ImVec2(10.0f, ImGui::GetIO().DisplaySize.y - 870.0f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_Always);

		ImGui::Begin("Selected Actor Details", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		auto it = actorMap.find(m_iSelectedActorID);
		if (it != actorMap.end())
		{
			auto actor = it->second;

			EditActorName(actor);
			EditActorTransform(actor);

			//if (ImGui::Button("Apply"))
			//{
				actor->SetPosition(m_vPosition);
				actor->SetRotation(m_vRotation);
				actor->SetScale(m_vScale);
				//actor->m_szName = m_szNewName;
			//}

			ImGui::SameLine();

			if (ImGui::Button("Delete"))
			{
				OBJECT->RemoveActor(actor);
				m_iSelectedActorID = -1;
			}

			ImGui::Separator(); ImGui::Spacing();

			std::string prefabPath = actor->GetPrefabPath();
			std::string extension;

			size_t pos = prefabPath.find_last_of('/');
			if (pos == std::string::npos) pos = prefabPath.find_last_of('\\');
			std::string filename = (pos == std::string::npos) ? prefabPath : prefabPath.substr(pos + 1);

			size_t extPos = filename.find_first_of('.');
			if (extPos != std::string::npos)
				extension = filename.substr(extPos);

			if (extension == ".map.json")
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Map Tile Info");

				auto tileActor = std::dynamic_pointer_cast<ATerrainTileActor>(actor);
				ViewMapData(tileActor);
				ImGui::Separator(); ImGui::Spacing();
				ViewActorInformation(tileActor);
				ImGui::Separator(); ImGui::Spacing();
				ViewMeshComponentData(tileActor);
				ImGui::Separator(); ImGui::Spacing();

				if (ImGui::Button("Save Map Prefab"))
					SaveMapPrefab(tileActor);
			}
			else if (extension == ".character.json")
			{
				ViewCharacterData(actor);
				ImGui::Separator(); ImGui::Spacing();
				ViewActorInformation(actor);
				ImGui::Separator(); ImGui::Spacing();
				EditShapeTransformData(actor);
				ImGui::Separator(); ImGui::Spacing();
				ViewShapeComponentData(actor);
				ImGui::Separator(); ImGui::Spacing();

				if (ImGui::Button("Save Character Prefab"))
					SaveCharacterPrefab(actor);
			}
			else if (extension == ".object.json" || extension == ".objects.json")
			{
				ViewActorInformation(actor);
				ImGui::Separator(); ImGui::Spacing();
				ViewMeshComponentData(actor);
				ImGui::Separator(); ImGui::Spacing();
				EditShapeTransformData(actor);
				ImGui::Separator(); ImGui::Spacing();
				ViewShapeComponentData(actor);
				ImGui::Separator(); ImGui::Spacing();

				ImGui::InputText("Save Name", m_szSaveName, IM_ARRAYSIZE(m_szSaveName));

				if (ImGui::Button("Save Selected Prefab"))
				{
					std::vector<std::shared_ptr<AActor>> selectedActors;
					const auto& ids = GetMultiSelectedIDs();

					for (UINT id : ids)
					{
						auto actor = OBJECT->GetActor(id);
						if (actor) selectedActors.push_back(actor);
					}

					std::string basePath = "../Resources/Prefab/";
					std::string fileName = m_szSaveName;

					if (selectedActors.size() == 1)
					{
						std::string fullPath = basePath + fileName + ".object.json";
						SaveObjectPrefab(selectedActors[0], fullPath);
					}
					else if (!selectedActors.empty())
					{
						std::string fullPath = basePath + fileName + ".objects.json";
						SaveObjectsPrefab(selectedActors, fullPath);
					}
				}
			}
		}
		ImGui::End();
	}

	m_vLastMultiSelectedIDs = m_vMultiSelectedIDs;
}


void ActorListUI::Toggle()
{
	m_bVisible = !m_bVisible;
}

void ActorListUI::ViewTransformData(const std::shared_ptr<AActor>& _actor)
{
	Vec3 pos = _actor->GetPosition();
	Vec3 rot = _actor->GetRotation();
	Vec3 scale = _actor->GetScale();

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
	ImGui::Text("Position: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
	ImGui::Text("Rotation: %.2f, %.2f, %.2f", rot.x, rot.y, rot.z);
	ImGui::Text("Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
}

void ActorListUI::ViewActorInformation(const std::shared_ptr<AActor>& _actor)
{
	std::wstring wName = _actor->m_szName;
	std::string name(wName.begin(), wName.end());
	std::string actorType = typeid(*_actor).name();

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Actor Info");
	ImGui::Text("Name: %s", name.c_str());
	ImGui::Text("Class Type: %s", actorType.c_str());
}

void ActorListUI::ViewMeshComponentData(const std::shared_ptr<AActor>& _actor)
{
	auto meshComp = _actor->GetMeshComponent<UMeshComponent>();
	bool isSkinned = std::dynamic_pointer_cast<USkinnedMeshComponent>(meshComp) != nullptr;

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh Info");
	ImGui::Text("Mesh Type : %s", isSkinned ? "Skinned" : "Static");

	if (meshComp)
	{
		std::wstring wMeshPath = meshComp->GetMeshPath();
		std::string meshPath(wMeshPath.begin(), wMeshPath.end());

		if (!meshPath.empty())
		{
			std::string extension = meshPath.substr(meshPath.find_last_of('.') + 1);

			std::string displayPath = meshPath;
			std::string baseFolder = "../Resources/";
			size_t pos = meshPath.find(baseFolder);
			if (pos != std::string::npos)
				displayPath = meshPath.substr(pos + baseFolder.length());

			ImGui::Text("Mesh Path : %s", displayPath.c_str());
		}
		else
		{
			ImGui::Text("Mesh Path: None");
		}

		auto material = meshComp->GetMaterial();
		if (material)
		{
			std::wstring wTex = material->GetTexturePath();
			std::wstring wShader = material->GetShaderPath();
			std::string texPath(wTex.begin(), wTex.end());
			std::string shaderPath(wShader.begin(), wShader.end());

			std::string displayTexPath = texPath;
			std::string displayShaderPath = shaderPath;

			const std::string basePath = "../Resources/";
			size_t texPos = texPath.find(basePath);
			if (texPos != std::string::npos)
				displayTexPath = texPath.substr(texPos + basePath.length());

			size_t shaderPos = shaderPath.find(basePath);
			if (shaderPos != std::string::npos)
				displayShaderPath = shaderPath.substr(shaderPos + basePath.length());

			ImGui::Separator(); ImGui::Spacing();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Material Info");
			ImGui::Text("Texture : %s", displayTexPath.c_str());
			ImGui::Text("Shader : %s", displayShaderPath.c_str());

			Vec3 spec = { 0.f,0.f,0.f };
			Vec3 emissive = { 0.f,0.f,0.f };
			float shininess = 0.f;
			float emissivePower = 0.f;

			ImGui::Text("SpecularColor : %.2f, %.2f, %.2f", spec.x, spec.y, spec.z);
			ImGui::Text("Shininess : %.2f", shininess);
			ImGui::Text("EmissiveColor : %.2f, %.2f, %.2f", emissive.x, emissive.y, emissive.z);
			ImGui::Text("EmissivePower : %.2f", emissivePower);
		}
	}
}

void ActorListUI::ViewShapeComponentData(const std::shared_ptr<AActor>& _actor)
{
	auto shapeComp = _actor->GetShapeComponent();
	if (shapeComp)
	{
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shape Info");

		ImGui::Text("isUse: true");

		const char* shapeTypeStr = "Unknown";
		switch (shapeComp->GetShapeType())
		{
		case ShapeType::ST_NONE:    shapeTypeStr = "None";	  break;
		case ShapeType::ST_BOX:     shapeTypeStr = "Box";	  break;
		case ShapeType::ST_SPHERE:  shapeTypeStr = "Sphere";  break;
		case ShapeType::ST_CAPSULE: shapeTypeStr = "Capsule"; break;
		}
		ImGui::Text("ShapeType: %s", shapeTypeStr);

		Vec3 sp = shapeComp->GetLocalPosition();
		Vec3 sr = shapeComp->GetLocalRotation();
		Vec3 ss = shapeComp->GetLocalScale();

		ImGui::Text("Shape Pos: %.2f, %.2f, %.2f", sp.x, sp.y, sp.z);
		ImGui::Text("Shape Rot: %.2f, %.2f, %.2f", sr.x, sr.y, sr.z);
		ImGui::Text("Shape Scl: %.2f, %.2f, %.2f", ss.x, ss.y, ss.z);
	}
	else
	{
		ImGui::Separator(); ImGui::Spacing();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shape Info");
		ImGui::Text("isUse: false");
	}
}

void ActorListUI::ViewMapData(const std::shared_ptr<ATerrainTileActor>& _tileactor)
{
	int cols = _tileactor->GetNumCols();
	int rows = _tileactor->GetNumRows();
	float size = _tileactor->GetCellSize();

	ImGui::Text("Cols: %d", cols);
	ImGui::Text("Rows: %d", rows);
	ImGui::Text("Cell Size: %.2f", size);

	ImGui::Text("Selected Row: %d", 0);
	ImGui::Text("Selected Col: %d", 0);
	ImGui::Text("Target Height: %.2f", 0);
}

void ActorListUI::ViewCharacterData(const std::shared_ptr<AActor>& _actor)
{
}

void ActorListUI::EditActorTransform(std::shared_ptr<class AActor>& _actor)
{
	m_vPosition = _actor->GetPosition();
	m_vRotation = _actor->GetRotation();
	m_vScale = _actor->GetScale();

	// SRT 조절 UI
	ImGui::InputFloat3("Position", &m_vPosition.x);
	ImGui::InputFloat3("Rotation", &m_vRotation.x);
	ImGui::InputFloat3("Scale", &m_vScale.x);

	// Gizmo Mode 선택 UI
	int srtMode = static_cast<int>(m_eGizmoMode);
	const char* srtLabels[] = { "Translate", "Rotate", "Scale" };

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Gizmo Mode (SRT)");

	if (ImGui::Combo("SRT Mode", &srtMode, srtLabels, IM_ARRAYSIZE(srtLabels)))
	{
		m_eGizmoMode = static_cast<GizmoMode>(srtMode);
	}

	// 단축키 입력 처리 (UI 갱신도 함께)
	bool ctrl = ImGui::GetIO().KeyCtrl;
	if (ctrl && ImGui::IsKeyPressed(ImGuiKey_W)) { m_eGizmoMode = GizmoMode::Translate; srtMode = 0; }
	if (ctrl && ImGui::IsKeyPressed(ImGuiKey_E)) { m_eGizmoMode = GizmoMode::Rotate;    srtMode = 1; }
	if (ctrl && ImGui::IsKeyPressed(ImGuiKey_R)) { m_eGizmoMode = GizmoMode::Scale;     srtMode = 2; }
}

void ActorListUI::EditActorName(std::shared_ptr<class AActor>& _actor)
{
	static char nameBuffer[64] = "";
	std::string currentName(_actor->m_szName.begin(), _actor->m_szName.end());
	strcpy_s(nameBuffer, currentName.c_str());

	if (ImGui::InputText("Edit Name", nameBuffer, IM_ARRAYSIZE(nameBuffer)))
	{
		std::wstring newName(nameBuffer, nameBuffer + strlen(nameBuffer));
		//m_szNewName = newName;
		_actor->m_szName = newName;
	}
}

void ActorListUI::EditShapeTransformData(std::shared_ptr<AActor>& _actor)
{
	auto shapeComp = _actor->GetShapeComponent();
	if (!shapeComp)
	{
		ImGui::Separator(); ImGui::Spacing();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shape Info");
		ImGui::Text("isUse: false");
		return;
	}

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shape Info");
	ImGui::Text("isUse: true");

	const char* shapeTypeStr = "Unknown";
	switch (shapeComp->GetShapeType())
	{
	case ShapeType::ST_NONE:    shapeTypeStr = "None";    break;
	case ShapeType::ST_BOX:     shapeTypeStr = "Box";     break;
	case ShapeType::ST_SPHERE:  shapeTypeStr = "Sphere";  break;
	case ShapeType::ST_CAPSULE: shapeTypeStr = "Capsule"; break;
	}
	ImGui::Text("ShapeType: %s", shapeTypeStr);

	Vec3 shapePos = shapeComp->GetLocalPosition();
	Vec3 shapeRot = shapeComp->GetLocalRotation();
	Vec3 shapeScl = shapeComp->GetLocalScale();

	bool updated = false;
	updated |= ImGui::InputFloat3("Shape Pos", &shapePos.x);
	updated |= ImGui::InputFloat3("Shape Rot", &shapeRot.x);
	updated |= ImGui::InputFloat3("Shape Scl", &shapeScl.x);

	if (updated)
	{
		shapeComp->SetLocalPosition(shapePos);
		shapeComp->SetLocalRotation(shapeRot);
		shapeComp->SetLocalScale(shapeScl);
	}
}

void ActorListUI::SaveMapPrefab(const std::shared_ptr<class ATerrainTileActor>& _tileactor)
{
	PrefabMapData data;
	data.Position = m_vPosition;
	data.Rotation = m_vRotation;
	data.Scale = m_vScale;
	data.Cols = _tileactor->GetNumCols();
	data.Rows = _tileactor->GetNumRows();
	data.CellSize = _tileactor->GetCellSize();
	data.SelectedRow = 0;
	data.SelectedCol = 0;
	data.TargetHeight = 0.0f;
	data.TexturePath = to_wm(_tileactor->GetMeshComponent()->GetMaterial()->GetTexturePath());
	data.ShaderPath = to_wm(_tileactor->GetMeshComponent()->GetMaterial()->GetShaderPath());

	PREFAB->SaveMapTile(data, _tileactor->GetPrefabPath());
}

void ActorListUI::SaveCharacterPrefab(const std::shared_ptr<class AActor>& _actor)
{
	PrefabCharacterData data;
	data.Name = to_wm(_actor->m_szName);
	data.MeshPath = "";
	data.RootMeshPath = "";
	data.TexturePath = "";
	data.ShaderPath = "";
	data.ScriptType = 0;
	data.AnimIndex = 0;
	data.AnimSpeed = 1.0f;

	data.transform.Position[0] = m_vPosition.x;
	data.transform.Position[1] = m_vPosition.y;
	data.transform.Position[2] = m_vPosition.z;
	data.transform.Rotation[0] = m_vRotation.x;
	data.transform.Rotation[1] = m_vRotation.y;
	data.transform.Rotation[2] = m_vRotation.z;
	data.transform.Scale[0] = m_vScale.x;
	data.transform.Scale[1] = m_vScale.y;
	data.transform.Scale[2] = m_vScale.z;

	auto shape = _actor->GetShapeComponent();
	if (shape)
	{
		data.shape.isUse = true;
		data.shape.eShapeType = shape->GetShapeType();

		Vec3 sp = shape->GetLocalPosition();
		Vec3 sr = shape->GetLocalRotation();
		Vec3 ss = shape->GetLocalScale();

		data.shape.Position[0] = sp.x;
		data.shape.Position[1] = sp.y;
		data.shape.Position[2] = sp.z;

		data.shape.Rotation[0] = sr.x;
		data.shape.Rotation[1] = sr.y;
		data.shape.Rotation[2] = sr.z;

		data.shape.Scale[0] = ss.x;
		data.shape.Scale[1] = ss.y;
		data.shape.Scale[2] = ss.z;
	}

	PREFAB->SaveCharacter(data, _actor->GetPrefabPath());
}

void ActorListUI::SaveObjectPrefab(const std::shared_ptr<class AActor>& _actor, const std::string& _path)
{
	PrefabObjectData data;
	data.Name = to_wm(_actor->m_szName);
	data.Position = m_vPosition;
	data.Rotation = m_vRotation;
	data.Scale = m_vScale;

	auto meshComp = _actor->GetMeshComponent<UMeshComponent>();
	if (meshComp)
	{
		data.MeshPath = to_wm(meshComp->GetMeshPath());

		auto mat = meshComp->GetMaterial();
		if (mat)
		{
			data.TexturePath = to_wm(mat->GetTexturePath());
			data.ShaderPath = to_wm(mat->GetShaderPath());
			data.SpecularColor = { 0.f,0.f,0.f };
			data.Shininess = 0.0f;
			data.EmissiveColor = { 0.f,0.f,0.f };
			data.EmissivePower = 0.0f;
		}
	}

	auto shape = _actor->GetShapeComponent();
	if (shape)
	{
		data.ShapeData.isUse = true;
		data.ShapeData.eShapeType = shape->GetShapeType();

		Vec3 sp = shape->GetLocalPosition();
		Vec3 sr = shape->GetLocalRotation();
		Vec3 ss = shape->GetLocalScale();

		data.ShapeData.Position[0] = sp.x;
		data.ShapeData.Position[1] = sp.y;
		data.ShapeData.Position[2] = sp.z;

		data.ShapeData.Rotation[0] = sr.x;
		data.ShapeData.Rotation[1] = sr.y;
		data.ShapeData.Rotation[2] = sr.z;

		data.ShapeData.Scale[0] = ss.x;
		data.ShapeData.Scale[1] = ss.y;
		data.ShapeData.Scale[2] = ss.z;
	}

	PREFAB->SaveObject(data, _path);
}

void ActorListUI::SaveObjectsPrefab(const std::vector<std::shared_ptr<AActor>>& _actors, const std::string& _path)
{
	std::vector<PrefabObjectData> vPrefabList;

	for (const auto& actor : _actors)
	{
		if (!actor) continue;

		PrefabObjectData data;
		data.Name = to_wm(actor->m_szName);
		data.Position = actor->GetPosition();
		data.Rotation = actor->GetRotation();
		data.Scale = actor->GetScale();

		auto meshComp = actor->GetMeshComponent<UMeshComponent>();
		if (meshComp)
		{
			data.MeshPath = to_wm(meshComp->GetMeshPath());

			auto mat = meshComp->GetMaterial();
			if (mat)
			{
				data.TexturePath = to_wm(mat->GetTexturePath());
				data.ShaderPath = to_wm(mat->GetShaderPath());
				data.SpecularColor = { 0.f,0.f,0.f };
				data.Shininess = 0.0f;
				data.EmissiveColor = { 0.f,0.f,0.f };
				data.EmissivePower = 0.0f;
			}
		}

		auto shape = actor->GetShapeComponent();
		if (shape)
		{
			data.ShapeData.isUse = true;
			data.ShapeData.eShapeType = shape->GetShapeType();

			Vec3 sp = shape->GetLocalPosition();
			Vec3 sr = shape->GetLocalRotation();
			Vec3 ss = shape->GetLocalScale();

			data.ShapeData.Position[0] = sp.x;
			data.ShapeData.Position[1] = sp.y;
			data.ShapeData.Position[2] = sp.z;

			data.ShapeData.Rotation[0] = sr.x;
			data.ShapeData.Rotation[1] = sr.y;
			data.ShapeData.Rotation[2] = sr.z;

			data.ShapeData.Scale[0] = ss.x;
			data.ShapeData.Scale[1] = ss.y;
			data.ShapeData.Scale[2] = ss.z;
		}

		vPrefabList.push_back(data);
	}

	if (!vPrefabList.empty())
	{
		PREFAB->SaveObjectArray(vPrefabList, _path);
	}
}

UINT ActorListUI::GetLastActorID()
{
	return m_iLastActorID;
}

UINT ActorListUI::GetSelectedActorID()
{
	return m_iSelectedActorID;
}

std::vector<UINT>& ActorListUI::GetMultiSelectedIDs()
{
	return m_vMultiSelectedIDs;
}

const std::vector<UINT>& ActorListUI::GetLastMultiSelectedIDs() const
{
	return m_vLastMultiSelectedIDs;
}

void ActorListUI::SetMultiSelectMode(bool _mode)
{
	m_bMultiSelectMode = _mode;
}

bool ActorListUI::IsMultiSelectMode() const
{
	return m_bMultiSelectMode;
}

void ActorListUI::RefreshSelectionVisuals()
{
	for (UINT id : m_vLastMultiSelectedIDs)
	{
		if (std::find(m_vMultiSelectedIDs.begin(), m_vMultiSelectedIDs.end(), id) == m_vMultiSelectedIDs.end())
		{
			auto it = OBJECT->GetActorList().find(id);
			if (it != OBJECT->GetActorList().end())
			{
				auto actor = it->second;
				auto mesh = actor->GetMeshComponent<UMeshComponent>();
				if (mesh && mesh->GetMaterial())
					mesh->GetMaterial()->SetGlowParams(0.f, Vec3(0.f, 0.f, 0.f));
			}
		}
	}

	for (UINT id : m_vMultiSelectedIDs)
	{
		auto it = OBJECT->GetActorList().find(id);
		if (it != OBJECT->GetActorList().end())
		{
			auto actor = it->second;
			auto mesh = actor->GetMeshComponent<UMeshComponent>();
			if (mesh && mesh->GetMaterial())
				mesh->GetMaterial()->SetGlowParams(0.7f, Vec3(1.0f, 0.0f, 0.0f));
		}
	}

	m_vLastMultiSelectedIDs = m_vMultiSelectedIDs;
	m_iLastActorID = m_iSelectedActorID;
}

UINT ActorListUI::GetSelectedGizmoAxis()
{
	return m_iSelectedGizmoAxis;
}

void ActorListUI::SetSelectedActorID(UINT id)
{
	m_iSelectedActorID = id;
}

void ActorListUI::SetSelectedGizmoAxis(UINT id)
{
	m_iSelectedGizmoAxis = id;
}
