#include "pch.h"
#include "ActorListUI.h"
#include "ObjectManager.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UMaterial.h"
#include "PrefabLoader.h"

void ActorListUI::DrawUI()
{
	if (!m_bVisible) return;

    ImGui::SetNextWindowPos(ImVec2(10.0f, ImGui::GetIO().DisplaySize.y - 600.0f - 60.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_Always);

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
                m_vPosition = actor->GetPosition();
                m_vRotation = actor->GetRotation();
                m_vScale = actor->GetScale();
            }


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

            if (ImGui::Button("Load Actor's Prefab"))
            {
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
                    PrefabMapData mapData;
                    if (PREFAB->LoadMapTile(prefabPath, mapData))
                    {
                        ImGui::Separator(); ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Map Tile Info");
                        ImGui::Text("Cols: %d, Rows: %d, CellSize: %.2f", mapData.Cols, mapData.Rows, mapData.CellSize);
                        ImGui::Text("Position: %.2f, %.2f, %.2f", mapData.Position.x, mapData.Position.y, mapData.Position.z);
                        ImGui::Text("Rotation: %.2f, %.2f, %.2f", mapData.Rotation.x, mapData.Rotation.y, mapData.Rotation.z);
                        ImGui::Text("Scale: %.2f, %.2f, %.2f", mapData.Scale.x, mapData.Scale.y, mapData.Scale.z);
                        ImGui::Text("Selected Row: %d, Col: %d, TargetHeight: %.2f", mapData.SelectedRow, mapData.SelectedCol, mapData.TargetHeight);
                        ImGui::Text("Texture: %s", mapData.TexturePath.c_str());
                        ImGui::Text("Shader: %s", mapData.ShaderPath.c_str());
                    }
                }
                else if (extension == ".character.json")
                {
                    PrefabCharacterData characterData;
                    if (PREFAB->LoadCharacter(prefabPath, characterData))
                    {
                        ImGui::Separator(); ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform (From Prefab)");
                        ImGui::Text("Position: %.2f, %.2f, %.2f", characterData.actor.Position[0], characterData.actor.Position[1], characterData.actor.Position[2]);
                        ImGui::Text("Rotation: %.2f, %.2f, %.2f", characterData.actor.Rotation[0], characterData.actor.Rotation[1], characterData.actor.Rotation[2]);
                        ImGui::Text("Scale: %.2f, %.2f, %.2f", characterData.actor.Scale[0], characterData.actor.Scale[1], characterData.actor.Scale[2]);

                        ImGui::Separator(); ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Character Info");
                        ImGui::Text("Name: %s", characterData.Name.c_str());
                        ImGui::Text("ScriptType: %d", characterData.ScriptType);
                        ImGui::Text("AnimIndex: %d", characterData.AnimIndex);
                        ImGui::Text("AnimSpeed: %.2f", characterData.AnimSpeed);

                        ImGui::Separator(); ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Camera Info");
                        ImGui::Text("isUse: %s", characterData.camera.isUse ? "true" : "false");
                        ImGui::Text("Position: %.2f, %.2f, %.2f", characterData.camera.Position[0], characterData.camera.Position[1], characterData.camera.Position[2]);
                        ImGui::Text("Rotation: %.2f, %.2f, %.2f", characterData.camera.Rotation[0], characterData.camera.Rotation[1], characterData.camera.Rotation[2]);
                        ImGui::Text("FOV: %.2f, Aspect: %.2f, Near: %.2f, Far: %.2f", characterData.camera.Fov, characterData.camera.Aspect, characterData.camera.Near, characterData.camera.Far);

                        ImGui::Separator(); ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shape Info");
                        ImGui::Text("isUse: %s", characterData.shape.isUse ? "true" : "false");
                        ImGui::Text("ShapeType: %d", characterData.shape.eShapeType);
                        ImGui::Text("Position: %.2f, %.2f, %.2f", characterData.shape.Position[0], characterData.shape.Position[1], characterData.shape.Position[2]);
                        ImGui::Text("Rotation: %.2f, %.2f, %.2f", characterData.shape.Rotation[0], characterData.shape.Rotation[1], characterData.shape.Rotation[2]);
                        ImGui::Text("Scale: %.2f, %.2f, %.2f", characterData.shape.Scale[0], characterData.shape.Scale[1], characterData.shape.Scale[2]);
                    }
                }
                else if (extension == ".object.json" || extension == ".objects.json")
                {
                    PrefabObjectData objectData;
                    if (PREFAB->LoadObject(prefabPath, objectData))
                    {
                        ImGui::Separator(); ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform (From Prefab)");
                        ImGui::Text("Position: %.2f, %.2f, %.2f", objectData.Position.x, objectData.Position.y, objectData.Position.z);
                        ImGui::Text("Rotation: %.2f, %.2f, %.2f", objectData.Rotation.x, objectData.Rotation.y, objectData.Rotation.z);
                        ImGui::Text("Scale: %.2f, %.2f, %.2f", objectData.Scale.x, objectData.Scale.y, objectData.Scale.z);

                        ImGui::Separator(); ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Material Info");
                        ImGui::Text("MeshPath: %s", objectData.MeshPath.c_str());
                        ImGui::Text("ShaderPath: %s", objectData.ShaderPath.c_str());
                        ImGui::Text("TexturePath: %s", objectData.TexturePath.c_str());
                        ImGui::Text("SpecularColor: %.2f, %.2f, %.2f", objectData.SpecularColor.x, objectData.SpecularColor.y, objectData.SpecularColor.z);
                        ImGui::Text("Shininess: %.2f", objectData.Shininess);
                        ImGui::Text("EmissiveColor: %.2f, %.2f, %.2f", objectData.EmissiveColor.x, objectData.EmissiveColor.y, objectData.EmissiveColor.z);
                        ImGui::Text("EmissivePower: %.2f", objectData.EmissivePower);

                        ImGui::Separator(); ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shape Info");
                        ImGui::Text("isUse: %s", objectData.ShapeData.isUse ? "true" : "false");
                        ImGui::Text("ShapeType: %d", objectData.ShapeData.eShapeType);
                        ImGui::Text("Position: %.2f, %.2f, %.2f", objectData.ShapeData.Position[0], objectData.ShapeData.Position[1], objectData.ShapeData.Position[2]);
                        ImGui::Text("Rotation: %.2f, %.2f, %.2f", objectData.ShapeData.Rotation[0], objectData.ShapeData.Rotation[1], objectData.ShapeData.Rotation[2]);
                        ImGui::Text("Scale: %.2f, %.2f, %.2f", objectData.ShapeData.Scale[0], objectData.ShapeData.Scale[1], objectData.ShapeData.Scale[2]);
                    }
                }
                else
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Unsupported prefab type: %s", prefabPath.c_str());
                }
            }
        }
    }
    //        // ---------------- Transform ----------------
    //        Vec3 pos = actor->GetPosition();
    //        Vec3 rot = actor->GetRotation();
    //        Vec3 scale = actor->GetScale();

    //        ImGui::Separator(); ImGui::Spacing();
    //        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
    //        ImGui::Text("Position: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
    //        ImGui::Text("Rotation: %.2f, %.2f, %.2f", rot.x, rot.y, rot.z);
    //        ImGui::Text("Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);

    //        // ---------------- Actor Info ----------------
    //        std::wstring wName = actor->m_szName;
    //        std::string name(wName.begin(), wName.end());
    //        std::string actorType = typeid(*actor).name();

    //        ImGui::Separator(); ImGui::Spacing();
    //        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Actor Info");
    //        ImGui::Text("Name: %s", name.c_str());
    //        ImGui::Text("Class Type: %s", actorType.c_str());

    //        // ---------------- Mesh Info ----------------
    //        auto meshComp = actor->GetMeshComponent<UMeshComponent>();
    //        bool isSkinned = std::dynamic_pointer_cast<USkinnedMeshComponent>(meshComp) != nullptr;

    //        ImGui::Separator(); ImGui::Spacing();
    //        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh Info");
    //        ImGui::Text("Mesh Type: %s", isSkinned ? "Skinned" : "Static");

    //        if (meshComp)
    //        {
    //            std::wstring wMeshPath = meshComp->GetMeshPath();
    //            std::string meshPath(wMeshPath.begin(), wMeshPath.end());

    //            if (!meshPath.empty())
    //            {
    //                std::string extension = meshPath.substr(meshPath.find_last_of('.') + 1);
    //                std::string typeLabel = (extension == "asset") ? "Asset" : (extension == "obj") ? "Obj" : "Unknown";
    //                ImGui::Text("Mesh Path (%s): %s", typeLabel.c_str(), meshPath.c_str());
    //            }
    //            else
    //            {
    //                ImGui::Text("Mesh Path: None");
    //            }

    //            auto material = meshComp->GetMaterial();
    //            if (material)
    //            {
    //                std::wstring wTex = material->GetTexturePath();
    //                std::wstring wShader = material->GetShaderPath();
    //                std::string texPath(wTex.begin(), wTex.end());
    //                std::string shaderPath(wShader.begin(), wShader.end());

    //                ImGui::Separator(); ImGui::Spacing();
    //                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Material Info");
    //                ImGui::Text("Texture: %s", texPath.c_str());
    //                ImGui::Text("Shader: %s", shaderPath.c_str());

    //                Vec3 spec = { 0.f,0.f,0.f };
    //                Vec3 emissive = { 0.f,0.f,0.f };
    //                float shininess = 0.f;
    //                float emissivePower = 0.f;

    //                ImGui::Text("SpecularColor: %.2f, %.2f, %.2f", spec.x, spec.y, spec.z);
    //                ImGui::Text("Shininess: %.2f", shininess);
    //                ImGui::Text("EmissiveColor: %.2f, %.2f, %.2f", emissive.x, emissive.y, emissive.z);
    //                ImGui::Text("EmissivePower: %.2f", emissivePower);
    //            }
    //        }

    //        // ---------------- Shape Info ----------------
    //        auto shapeComp = actor->GetShapeComponent();
    //        if (shapeComp)
    //        {
    //            ImGui::Separator(); ImGui::Spacing();
    //            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shape Info");

    //            ImGui::Text("isUse: true");
    //            ImGui::Text("ShapeType: %d", static_cast<int>(shapeComp->GetShapeType()));

    //            Vec3 sp = shapeComp->GetLocalPosition();
    //            Vec3 sr = shapeComp->GetLocalRotation();
    //            Vec3 ss = shapeComp->GetLocalScale();

    //            ImGui::Text("Shape Pos: %.2f, %.2f, %.2f", sp.x, sp.y, sp.z);
    //            ImGui::Text("Shape Rot: %.2f, %.2f, %.2f", sr.x, sr.y, sr.z);
    //            ImGui::Text("Shape Scl: %.2f, %.2f, %.2f", ss.x, ss.y, ss.z);
    //        }
    //        else
    //        {
    //            ImGui::Separator(); ImGui::Spacing();
    //            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shape Info");
    //            ImGui::Text("isUse: false");
    //        }
    //    }
    //}

    ImGui::End();
}

void ActorListUI::Toggle()
{
	m_bVisible = !m_bVisible;
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
