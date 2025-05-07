#include "pch.h"
#include "ObjectModifyUI.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"
#include "UAnimInstance.h"
#include "UMaterial.h"

void ObjectModifyUI::DrawUI()
{
    // 메뉴 옵션
    if (ImGui::Button("Asset")) { currentTab = EMenuTab::Asset; }
    ImGui::SameLine();
    if (ImGui::Button("Texture")) { currentTab = EMenuTab::Texture; }
    ImGui::SameLine();
    if (ImGui::Button("Mesh")) { currentTab = EMenuTab::Mesh; }
    ImGui::Separator();

    // 요소 사이즈
    ImVec2 itemSize(64, 64);

    switch (currentTab)
    {
    case EMenuTab::Actor:
        ImGui::Text("Actor :");
        if (ImGui::Button("Add MeshComponent"))
        {
            m_bEditMesh = true;
        }
        break;
    case EMenuTab::Mesh:
        ImGui::Text("Mesh :");
        for (size_t i = 0; i < m_vMeshList.size(); i++)
        {
            ImGui::PushID(static_cast<int>(i));
            ImGui::BeginGroup();
            string name = to_wm(m_vMeshList[i]->GetName());
            ImGui::Button(name.c_str(), itemSize);
            //if (ImGui::Button(name.c_str(), itemSize) && m_OnAddMeshApply)
            //{
            //    m_OnAddMeshApply(targetNum, i);
            //}
            ImGui::TextWrapped("%s", name.c_str());
            ImGui::EndGroup();
            ImGui::PopID();

            if ((i + 1) % 3 != 0) ImGui::SameLine();
        }
        break;
    case EMenuTab::Texture:
        ImGui::Text("Texture :");
        for (size_t i = 0; i < m_vTextureList.size(); ++i)
        {
            ImGui::PushID(static_cast<int>(i));
            ImGui::BeginGroup();
            ImGui::Image((ImTextureID)(m_vTextureList[i]->m_pTexSRV.Get()), ImVec2((float)64, (float)64));
            //ImGui::Button(textureList[i].name.c_str(), itemSize);
            //ImGui::TextWrapped("%s", textureList[i].name.c_str());
            ImGui::EndGroup();
            ImGui::PopID();

            if ((i + 1) % 5 != 0) ImGui::SameLine();
        }
        break;

    case EMenuTab::Asset:
        ImGui::Text("Asset :");
        //for (size_t i = 0; i < assetList.size(); ++i)
        //{
        //    //if (!assetList[i].m_pTexSRV) continue;

        //    //ImGui::PushID(static_cast<int>(i));
        //    //ImGui::BeginGroup();
        //    //ImGui::ImageButton((ImTextureID)assetList[i].m_pTexSRV.Get(), itemSize);
        //    //ImGui::TextWrapped("Texture %zu", i); // 이름이 없을 경우 인덱스로 대체
        //    //ImGui::EndGroup();
        //    //ImGui::PopID();

        //    if ((i + 1) % 4 != 0) ImGui::SameLine();
        //}
        break;

    }

    if (m_bEditMesh)
    {
        DrawMeshUI();
    }

    //ImGui::End();
}

void ObjectModifyUI::DrawMeshUI()
{
    ImGui::Begin("Mesh Edit", &m_bEditMesh);
    ImGui::Text("Select Mesh Type");
    ImGui::End();
}
