#include "pch.h"
#include "CharacterEditorUI.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"

bool CharacterEditorUI::m_bRootSet = false;
bool CharacterEditorUI::m_bChildAttached = false;
// *Crow 생성되자마자 망토 같이 만들면서 Child가 생김
int CharacterEditorUI::m_iChildIndex = 1; // 0

void CharacterEditorUI::DrawUI()
{
    static std::vector<std::string> meshDisplayNames;
    static std::vector<const char*> meshDisplayNamePtrs;

    // Asset Load
    ImGui::InputText("Asset Path", m_szAssetPath, IM_ARRAYSIZE(m_szAssetPath));

    if (ImGui::Button("Load Asset"))
    {
        meshDisplayNames.clear();
        meshDisplayNamePtrs.clear();

        if (!m_pLoader)
        {
            m_pLoader = std::make_shared<ActorLoader>();
        }
        m_pLoader->LoadOne(m_szAssetPath);
        m_vMeshList = m_pLoader->LoadMesh();
    }

    // Mesh List 순회
    if (!m_vMeshList.empty())
    {
        meshDisplayNames.clear();
        meshDisplayNamePtrs.clear();

        for (size_t i = 0; i < m_vMeshList.size(); ++i)
        {
            auto mesh = m_vMeshList[i];
            // *isSkinned Mesh에 설정되어있지 않나?
            bool isSkinned = (std::dynamic_pointer_cast<USkinnedMeshComponent>(mesh) != nullptr);

            std::wstring wname = mesh->GetName();
            std::string name(wname.begin(), wname.end());
            name += isSkinned ? " (Skinned)" : " (Static)";

            meshDisplayNames.push_back(name);
            meshDisplayNamePtrs.push_back(meshDisplayNames.back().c_str());
        }

        ImGui::Combo("Mesh List", &m_iSelectedMeshIndex, meshDisplayNamePtrs.data(), (int)meshDisplayNamePtrs.size());
    }

    if (m_pLoader && !m_pLoader->m_vAnimInstanceList.empty())
    {
        auto animInstance = m_pLoader->m_vAnimInstanceList[0];
        const auto& animList = animInstance->GetAnimTrackList();

        static std::vector<std::string> animNames;
        static std::vector<const char*> animNamePtrs;
        animNames.clear();
        animNamePtrs.clear();

        for (const auto& anim : animList)
        {
            std::wstring wname = anim.m_szName;
            std::string name(wname.begin(), wname.end());
            animNames.push_back(name);
            animNamePtrs.push_back(animNames.back().c_str());
        }
        ImGui::Combo("Anim", &m_iSelectedAnimIndex, animNamePtrs.data(), (int)animNamePtrs.size());

        static float tempAnimPlayRate = 1.0f;
        if (tempAnimPlayRate == 1.0f)
            tempAnimPlayRate = animInstance->m_fAnimPlayRate;

        ImGui::SliderFloat("Anim Speed", &tempAnimPlayRate, 1.0f, 60.0f, "%.1f");
        animInstance->m_fAnimPlayRate = tempAnimPlayRate;

        animInstance->SetCurrentAnimTrack(m_iSelectedAnimIndex);
    }

    // Child Mesh 설정
    if (!m_vMeshList.empty())
    {
        if (ImGui::Button("Set As Root Component"))
        {
            m_pRootComponent = m_vMeshList[m_iSelectedMeshIndex];

            if (m_pRootComponent)
            {
                m_bRootSet = true;
            }
        }
    }
    if (m_bRootSet)
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Root component set.");
    }

    if (m_pRootComponent)
    {
        ImGui::Separator();

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Attach Child Mesh");

        if (!m_vMeshList.empty())
        {
            ImGui::Combo("Child Mesh", &m_iSelectedChildMeshIndex, meshDisplayNamePtrs.data(), (int)meshDisplayNamePtrs.size());
        }

        auto skinnedRoot = std::dynamic_pointer_cast<USkinnedMeshComponent>(m_pRootComponent);
        auto staticRoot = std::dynamic_pointer_cast<UStaticMeshComponent>(m_pRootComponent);

        if (skinnedRoot && skinnedRoot->GetMesh())
        {
            const auto& boneMap = skinnedRoot->GetMesh()->GetSkeletonList();
            static std::vector<std::string> boneNames;
            static std::vector<const char*> boneNamePtrs;

            boneNames.clear();
            boneNamePtrs.clear();

            for (auto it = boneMap.begin(); it != boneMap.end(); ++it)
            {
                const auto& boneNameW = it->first;
                const auto& boneNode = it->second;

                std::string boneName(boneNameW.begin(), boneNameW.end());
                boneNames.push_back(boneName);
                boneNamePtrs.push_back(boneNames.back().c_str());
            }

            static int selectedBoneIndex = 0;
            if (!boneNamePtrs.empty())
            {
                ImGui::Combo("Attach Bone", &selectedBoneIndex, boneNamePtrs.data(), (int)boneNamePtrs.size());
            }

            if (ImGui::Button("Attach To Root"))
            {
                auto parent = m_pRootComponent;
                auto childMesh = m_vMeshList[m_iSelectedChildMeshIndex];

                bool isSkinnedChild = std::dynamic_pointer_cast<USkinnedMeshComponent>(childMesh) != nullptr;

                if (isSkinnedChild)
                {
                    auto skinnedChild = std::dynamic_pointer_cast<USkinnedMeshComponent>(childMesh);

                    if (auto skinnedParent = std::dynamic_pointer_cast<USkinnedMeshComponent>(parent))
                    {
                        auto anim = skinnedParent->GetAnimInstance();
                        skinnedChild->SetBaseAnim(anim);
                    }

                    parent->AddChild(skinnedChild);

                    if (m_pRootComponent->GetChild(m_iChildIndex++))
                    {
                        m_bChildAttached = true;
                    }
                }
                else
                {
                    auto staticChild = std::dynamic_pointer_cast<UStaticMeshComponent>(childMesh);

                    if (auto skinnedParent = std::dynamic_pointer_cast<USkinnedMeshComponent>(parent))
                    {
                        auto anim = skinnedParent->GetAnimInstance();
                        staticChild->SetAnimInstance(anim);
                        staticChild->SetTargetBoneIndex(selectedBoneIndex);
                    }

                    parent->AddChild(staticChild);

                    if (m_pRootComponent->GetChild(m_iChildIndex++))
                    {
                        m_bChildAttached = true;
                    }
                }
            }
        }
        else if (staticRoot)
        {
            if (ImGui::Button("Attach To Root"))
            {
                auto child = m_vMeshList[m_iSelectedMeshIndex];
                staticRoot->AddChild(child);
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "Child attached to root.");
            }
        }
    }

    if (m_bChildAttached)
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Skinned child attached to root.");
    }

    // Material 설정
    if (!m_vMeshList.empty())
    {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Material Configuration");

        ImGui::InputText("Texture File", m_szTextureName, IM_ARRAYSIZE(m_szTextureName));
        ImGui::InputText("Shader File", m_szShaderName, IM_ARRAYSIZE(m_szShaderName));
    }

    // 스크립트 설정
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Script Configuration");

    // 여기에 스크립트 리스트 넣어야함(이름 정확히)
    static std::vector<std::string> scriptNames = { "None", "PlayerMoveScript", "EnemyAIScript" };
    static std::vector<const char*> scriptNamePtrs;
    if (scriptNamePtrs.empty())
    {
        for (auto& name : scriptNames)
            scriptNamePtrs.push_back(name.c_str());
    }

    ImGui::Combo("Script", &m_iSelectedScriptIndex, scriptNamePtrs.data(), (int)scriptNamePtrs.size());

    // SRT 적용
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
    DrawVec3Slider("Position", m_fPosition, -100.f, 100.f);
    DrawVec3Slider("Rotation", m_fRotation, -360.f, 360.f);
    DrawVec3Slider("Scale", m_fScale, 0.01f, 10.f);

    if (ImGui::Button("Finalize Actor Creation"))
    {
        if (m_pRootComponent && m_OnCreate)
        {
            Vec3 position(m_fPosition[0], m_fPosition[1], m_fPosition[2]);
            Vec3 rotation(m_fRotation[0], m_fRotation[1], m_fRotation[2]);
            Vec3 scale(m_fScale[0], m_fScale[1], m_fScale[2]);

            m_OnCreate(m_pRootComponent, position, rotation, scale, m_iSelectedScriptIndex);
        }

        m_bRootSet = false;
        m_bChildAttached = false;
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Character Save/Load");

    static char characterSavePath[256] = "../Resources/Prefab/MyCharacter.prefab.json";
    ImGui::InputText("Character Path##Save", characterSavePath, IM_ARRAYSIZE(characterSavePath));

    if (ImGui::Button("Save##Character"))
    {
        PrefabCharacterData data;
        data.Name = "MyCharacter";
        data.RootMeshPath = m_szAssetPath;
        data.ShaderPath = m_szShaderName;
        data.TexturePath = m_szTextureName;
        data.ScriptType = m_iSelectedScriptIndex;
        data.AnimIndex = m_iSelectedAnimIndex;

        auto animInstance = m_pLoader->m_vAnimInstanceList.empty() ? nullptr : m_pLoader->m_vAnimInstanceList[0];
        data.AnimSpeed = animInstance ? animInstance->m_fAnimPlayRate : 1.0f;

        data.Scale = Vec3(m_fScale[0], m_fScale[1], m_fScale[2]);
        data.Rotation = Vec3(m_fRotation[0], m_fRotation[1], m_fRotation[2]);
        data.Translation = Vec3(m_fPosition[0], m_fPosition[1], m_fPosition[2]);

        // 자식 메쉬 정보 수집 (필요 시 확장 가능)
        for (int i = 0; i < m_iChildIndex; ++i)
        {
            auto child = m_pRootComponent->GetChild(i);
            if (child)
            {
                PrefabCharacterData::ChildMeshData childData;
                // 경로 정보는 없으니 이름을 임시 저장하거나, 로직 확장 필요
                childData.MeshPath = "Unknown";
                childData.TargetBoneIndex = m_iSelectedBoneIndex;
                data.ChildMeshes.push_back(childData);
            }
        }

        PREFAB->SaveCharacter(data, characterSavePath);
    }

    static char characterLoadPath[256] = "../Resources/Prefab/MyCharacter.prefab.json";
    ImGui::InputText("Character Path##Load", characterLoadPath, IM_ARRAYSIZE(characterLoadPath));

    if (ImGui::Button("Load##Character"))
    {
        PrefabCharacterData data;
        if (PREFAB->LoadCharacter(characterLoadPath, data))
        {
            // 에셋 경로 적용
            strcpy_s(m_szAssetPath, data.RootMeshPath.c_str());

            // 쉐이더 / 텍스처 경로 적용
            strcpy_s(m_szShaderName, data.ShaderPath.c_str());
            strcpy_s(m_szTextureName, data.TexturePath.c_str());

            // 스크립트, 애니메이션 설정
            m_iSelectedScriptIndex = data.ScriptType;
            m_iSelectedAnimIndex = data.AnimIndex;

            // 애니메이션 속도는 로더에서 추후 적용 필요
            auto animInstance = m_pLoader->m_vAnimInstanceList.empty() ? nullptr : m_pLoader->m_vAnimInstanceList[0];
            if (animInstance)
            {
                animInstance->m_fAnimPlayRate = data.AnimSpeed;
            }

            // Transform 적용
            m_fScale[0] = data.Scale.x; m_fScale[1] = data.Scale.y; m_fScale[2] = data.Scale.z;
            m_fRotation[0] = data.Rotation.x; m_fRotation[1] = data.Rotation.y; m_fRotation[2] = data.Rotation.z;
            m_fPosition[0] = data.Translation.x; m_fPosition[1] = data.Translation.y; m_fPosition[2] = data.Translation.z;

            // 캐릭터 로드 후 메시 로드 실행 유도
            if (!m_pLoader)
            {
                m_pLoader = std::make_shared<ActorLoader>();
            }
            m_pLoader->LoadOne(m_szAssetPath);
            m_vMeshList = m_pLoader->LoadMesh();
            m_iSelectedMeshIndex = 0;

            // 자식 메쉬는 현재 로직 상에서는 경로 기반이 아니라 추가 로직 필요
            // 향후 확장 예정
        }
    }

    ImGui::Separator();

    // 새로운 캐릭터 생성 시 초기화
    if (ImGui::Button("Reset"))
    {
        m_pRootComponent = nullptr;

        m_iSelectedMeshIndex = 0;
        m_iSelectedChildMeshIndex = 0;
        m_iSelectedAnimIndex = 0;
        m_iSelectedScriptIndex = 0;

        m_iChildIndex = 0;
    }
}

void CharacterEditorUI::DrawVec3Slider(const char* label, float* values, float minVal, float maxVal)
{
    ImGui::PushID(label);
    ImGui::Text("%s", label);
    ImGui::Indent(10.0f);

    static const char* axes[] = { "X", "Y", "Z" };
    const float buttonSize = 20.0f;
    const float inputWidth = 60.0f;
    const float sliderWidth = 150.0f;
    const float spacing = 4.0f;

    for (int i = 0; i < 3; ++i)
    {
        ImGui::PushID(i);

        ImGui::Text("%s", axes[i]); ImGui::SameLine();

        if (ImGui::Button("-", ImVec2(buttonSize, 0)))
            values[i] -= 0.1f;
        ImGui::SameLine(0, spacing);

        ImGui::PushItemWidth(sliderWidth);
        if (ImGui::SliderFloat("##Slider", &values[i], minVal, maxVal))
        {
            // 값이 바뀐 경우 추가 작업 가능
        }
        ImGui::PopItemWidth();

        ImGui::SameLine(0, spacing);

        if (ImGui::Button("+", ImVec2(buttonSize, 0)))
            values[i] += 0.1f;

        ImGui::SameLine(0, spacing);

        ImGui::PushItemWidth(inputWidth);
        ImGui::InputFloat("##Value", &values[i], 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopID();
    }

    ImGui::Unindent(10.0f);
    ImGui::PopID();
}

