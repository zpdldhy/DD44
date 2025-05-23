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

	ImGui::SetNextWindowPos(ImVec2(10.0f, ImGui::GetIO().DisplaySize.y - 360.0f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Always);

	ImGui::Begin("Actor List", &m_bVisible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	for (auto it = OBJECT->GetActorList().begin(); it != OBJECT->GetActorList().end(); ++it)
	{
		UINT id = it->first;
		auto actor = it->second;

		std::wstring wName = actor->m_szName;
		std::string name(wName.begin(), wName.end());

		std::string label = "Actor " + std::to_string(id);
		if (!name.empty())
			label += " - " + name;

		if (ImGui::Selectable(label.c_str(), m_iSelectedActorID == id))
		{
			m_iSelectedActorID = id;
		}
	}

	ImGui::Separator(); ImGui::Spacing();

	if (m_iSelectedActorID >= 0)
	{
		ImGui::SetNextWindowPos(ImVec2(10.0f, ImGui::GetIO().DisplaySize.y - 870.0f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Always);

		ImGui::Begin("Selected Actor Details", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		auto& actorMap = OBJECT->GetActorList();
		auto it = actorMap.find(m_iSelectedActorID);
		if (it != actorMap.end())
		{
			auto actor = it->second;

			// 선택 변경 감지
			if (m_iSelectedActorID != m_iLastActorID)
			{
				if (m_iLastActorID >= 0)
				{
					auto prevIt = actorMap.find(m_iLastActorID);
					if (prevIt != actorMap.end())
					{
						auto prevActor = prevIt->second;
						auto prevMeshComp = prevActor->GetMeshComponent<UMeshComponent>();
						if (prevMeshComp && prevMeshComp->GetMaterial())
							prevMeshComp->GetMaterial()->SetGlowParams(0.0f, Vec3(0.0f, 0.0f, 0.0f));
					}
				}

				auto meshComp = actor->GetMeshComponent<UMeshComponent>();
				if (meshComp && meshComp->GetMaterial())
					meshComp->GetMaterial()->SetGlowParams(0.7f, Vec3(1.0f, 0.0f, 0.0f));

				m_iLastActorID = m_iSelectedActorID;
			}

			m_vPosition = actor->GetPosition();
			m_vRotation = actor->GetRotation();
			m_vScale = actor->GetScale();

			// SRT 조절 UI
			ImGui::InputFloat3("Position", &m_vPosition.x);
			ImGui::InputFloat3("Rotation", &m_vRotation.x);
			ImGui::InputFloat3("Scale", &m_vScale.x);

			static char nameBuffer[64] = "";
			std::string currentName(actor->m_szName.begin(), actor->m_szName.end());
			strcpy_s(nameBuffer, currentName.c_str());

			if (ImGui::InputText("Edit Name", nameBuffer, IM_ARRAYSIZE(nameBuffer)))
			{
				std::wstring newName(nameBuffer, nameBuffer + strlen(nameBuffer));
				m_szNewName = newName;
			}

			if (ImGui::Button("Apply"))
			{
				actor->SetPosition(m_vPosition);
				actor->SetRotation(m_vRotation);
				actor->SetScale(m_vScale);

				actor->m_szName = m_szNewName;
			}

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

				// ---------------- Transform ----------------
				//ViewTransformData(tileActor);
				//ImGui::Separator(); ImGui::Spacing();

				// ----------------- Map Data -----------------
				ViewMapData(tileActor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Actor Info ----------------
				ViewActorInformation(tileActor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Mesh Info ----------------
				ViewMeshComponentData(tileActor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Shape Info ----------------
				ViewShapeComponentData(tileActor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Save Prefab ----------------
				if (ImGui::Button("Save Map Prefab"))
				{
					SaveMapPrefab(tileActor);
				}
			}
			else if (extension == ".character.json")
			{
				// ---------------- Transform ----------------
				//ViewTransformData(actor);
				//ImGui::Separator(); ImGui::Spacing();

				// -------------- Character Data --------------
				ViewCharacterData(actor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Actor Info ----------------
				ViewActorInformation(actor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Mesh Info ----------------
				ViewMeshComponentData(actor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Shape Info ----------------
				ViewShapeComponentData(actor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Save Prefab ----------------
				if (ImGui::Button("Save Character Prefab"))
				{
					SaveCharacterPrefab(actor);
				}
			}
			else if (extension == ".object.json" || extension == ".objects.json")
			{
				// ---------------- Transform ----------------
				//ViewTransformData(actor);
				//ImGui::Separator(); ImGui::Spacing();

				// ---------------- Actor Info ----------------
				ViewActorInformation(actor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Mesh Info ----------------
				ViewMeshComponentData(actor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Shape Info ----------------
				ViewShapeComponentData(actor);
				ImGui::Separator(); ImGui::Spacing();

				// ---------------- Save Prefab ----------------
				if (extension == ".object.json")
				{
					if (ImGui::Button("Save Object Prefab"))
					{
						SaveObjectPrefab(actor);
					}
				}
				else
				{
					// 다중 선택 및 다중 프리팹 저장
				}
			}
		}

		ImGui::End();
	}

	ImGui::End();
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
	ImGui::Text("Mesh Type: %s", isSkinned ? "Skinned" : "Static");

	if (meshComp)
	{
		std::wstring wMeshPath = meshComp->GetMeshPath();
		std::string meshPath(wMeshPath.begin(), wMeshPath.end());

		if (!meshPath.empty())
		{
			std::string extension = meshPath.substr(meshPath.find_last_of('.') + 1);
			std::string typeLabel = (extension == "asset") ? "Asset" : (extension == "obj") ? "Obj" : "Unknown";
			ImGui::Text("Mesh Path (%s): %s", typeLabel.c_str(), meshPath.c_str());
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

			ImGui::Separator(); ImGui::Spacing();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Material Info");
			ImGui::Text("Texture: %s", texPath.c_str());
			ImGui::Text("Shader: %s", shaderPath.c_str());

			Vec3 spec = { 0.f,0.f,0.f };
			Vec3 emissive = { 0.f,0.f,0.f };
			float shininess = 0.f;
			float emissivePower = 0.f;

			ImGui::Text("SpecularColor: %.2f, %.2f, %.2f", spec.x, spec.y, spec.z);
			ImGui::Text("Shininess: %.2f", shininess);
			ImGui::Text("EmissiveColor: %.2f, %.2f, %.2f", emissive.x, emissive.y, emissive.z);
			ImGui::Text("EmissivePower: %.2f", emissivePower);
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

	data.actor.Position[0] = m_vPosition.x;
	data.actor.Position[1] = m_vPosition.y;
	data.actor.Position[2] = m_vPosition.z;
	data.actor.Rotation[0] = m_vRotation.x;
	data.actor.Rotation[1] = m_vRotation.y;
	data.actor.Rotation[2] = m_vRotation.z;
	data.actor.Scale[0] = m_vScale.x;
	data.actor.Scale[1] = m_vScale.y;
	data.actor.Scale[2] = m_vScale.z;

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

void ActorListUI::SaveObjectPrefab(const std::shared_ptr<class AActor>& _actor)
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

	PREFAB->SaveObject(data, _actor->GetPrefabPath());
}

UINT ActorListUI::GetLastActorID()
{
	return m_iLastActorID;
}

UINT ActorListUI::GetSelectedActorID()
{
	return m_iSelectedActorID;
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
