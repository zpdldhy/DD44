#include "pch.h"
#include "ActorListUI.h"
#include "ObjectManager.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UMaterial.h"

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

            if (ImGui::Button("Apply Transform"))
            {
                actor->SetPosition(m_vPosition);
                actor->SetRotation(m_vRotation);
                actor->SetScale(m_vScale);
            }

            // 기본 정보
            std::wstring wName = actor->m_szName;
            std::string name(wName.begin(), wName.end());
            ImGui::Text("Name: %s", name.c_str());

            // Actor 종류
            std::string actorType = typeid(*actor).name();
            ImGui::Text("Type: %s", actorType.c_str());

            // Mesh 정보
            auto meshComp = actor->GetMeshComponent<UMeshComponent>();
            bool isSkinned = std::dynamic_pointer_cast<USkinnedMeshComponent>(meshComp) != nullptr;
            ImGui::Text("Mesh: %s", isSkinned ? "Skinned" : "Static");

            // Material 정보
            if (meshComp)
            {
                auto material = meshComp->GetMaterial();
                if (material)
                {
                    std::wstring wTexturePath = material->GetTexturePath();
                    std::wstring wShaderPath = material->GetShaderPath();

                    std::string texturePath(wTexturePath.begin(), wTexturePath.end());
                    std::string shaderPath(wShaderPath.begin(), wShaderPath.end());

                    ImGui::Text("Texture: %s", texturePath.c_str());
                    ImGui::Text("Shader: %s", shaderPath.c_str());

                    std::wstring wMeshPath = meshComp->GetMeshPath();
                    std::string meshPath(wMeshPath.begin(), wMeshPath.end());

                    if (!meshPath.empty())
                    {
                        std::string extension = meshPath.substr(meshPath.find_last_of('.') + 1);
                        std::string typeLabel = (extension == "asset") ? "Asset" : (extension == "obj") ? "Obj" : "Unknown";
                        ImGui::Text("%s: %s", typeLabel.c_str(), meshPath.c_str());
                    }
                    else
                    {
                        ImGui::Text("Mesh Path: None");
                    }
                }
            }

            // DragMode
            ImGui::Checkbox("Drag Mode", &m_bDragMode);

            // Transform 정보
            Vec3 pos = actor->GetPosition();
            Vec3 rot = actor->GetRotation();
            Vec3 scale = actor->GetScale();
            
            ImGui::Text("Position: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
            ImGui::Text("Rotation: %.2f, %.2f, %.2f", rot.x, rot.y, rot.z);
            ImGui::Text("Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);

            static char nameBuffer[64] = "";
            std::string currentName(actor->m_szName.begin(), actor->m_szName.end());
            strcpy_s(nameBuffer, currentName.c_str());

            if (ImGui::InputText("Edit Name", nameBuffer, IM_ARRAYSIZE(nameBuffer)))
            {
                std::wstring newName(nameBuffer, nameBuffer + strlen(nameBuffer));
                actor->m_szName = newName;
            }

            if (ImGui::Button("Delete Actor"))
            {
                OBJECT->RemoveActor(actor);
                m_iSelectedActorID = -1;
            }
        }
    }

    ImGui::End();
}

void ActorListUI::Toggle()
{
	m_bVisible = !m_bVisible;
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
