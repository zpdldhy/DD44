#include "pch.h"
#include "ObjectEditorUI.h"


void ObjectEditorUI::Update()
{
    ImGui::Begin("Asset Browser");

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
    case EMenuTab::Texture:
        ImGui::Text("Texture :");
        if (ImGui::Button("Temp") && m_OnTargetNumApply)
        {
            if (++targetNum >= objectMaxNum)
            {
                targetNum = 0;
            }
            m_OnTargetNumApply(targetNum);
        }
        //for (size_t i = 0; i < textureList.size(); ++i)
        //{
        //    ImGui::PushID(static_cast<int>(i));
        //    ImGui::BeginGroup();
        //    ImGui::Image((ImTextureID)(textureList[i]->m_pTexSRV.Get()), ImVec2((float)64, (float)64));
        //    //ImGui::Button(textureList[i].name.c_str(), itemSize);
        //    //ImGui::TextWrapped("%s", textureList[i].name.c_str());
        //    ImGui::EndGroup();
        //    ImGui::PopID();

        //    if ((i + 1) % 4 != 0) ImGui::SameLine();
        //}
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

    case EMenuTab::Mesh:
        ImGui::Text("Mesh :");
        for (size_t i = 0; i < meshNum; ++i)
        {
            ImGui::PushID(static_cast<int>(i));
            ImGui::BeginGroup();
            string name = "mesh" + to_string(i);
            //ImGui::Button(name.c_str(), itemSize);
            if (ImGui::Button(name.c_str(), itemSize) && m_OnAddMeshApply)
            {
                m_OnAddMeshApply(targetNum, i);
            }
            ImGui::TextWrapped("%s", name.c_str());
            ImGui::EndGroup();
            ImGui::PopID();

            if ((i + 1) % 4 != 0) ImGui::SameLine();
        }
        break;
    }

    ImGui::End();

}
