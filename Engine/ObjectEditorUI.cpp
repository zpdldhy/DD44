#include "pch.h"
#include "ObjectEditorUI.h"
#include "PrefabLoader.h"

void ObjectEditorUI::DrawUI()
{
    // ────────────────────────── Mesh Browser  ─────────────────────────────
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Browse Mesh Files");

    ImGui::BeginChild("FileExplorer", ImVec2(0, 200), true);
    StartFileBrowser("../Resources/Asset/", "");
    ImGui::EndChild();

    ImGui::Checkbox("Placement Mode", &m_bPlacementMode);

    ImGui::Separator(); ImGui::Spacing();

    // ────────────────────────── Asset Paths ─────────────────────────────
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Asset Paths");
    ImGui::InputText("Asset", m_szObjPath, IM_ARRAYSIZE(m_szObjPath));

    ImGui::Separator(); ImGui::Spacing();

    // ──────────────────────────── Transform ─────────────────────────────
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
    DrawVec3Slider("Position", m_fPosition, -100.f, 100.f);
    DrawVec3Slider("Rotation", m_fRotation, -360.f, 360.f);
    DrawVec3Slider("Scale", m_fScale, 0.01f, 10.f);

    ImGui::Spacing();

    ImGui::Checkbox("Snap to Grid", &m_bSnapEnabled);

    ImGui::Separator(); ImGui::Spacing();

    // ───────────────────────────── Lighting ─────────────────────────────-
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "SpecularColor");
    DrawVec3Slider("SpecularColor", m_fSpecularColor, 0.f, 1.f);
    ImGui::InputFloat("Shininess", &m_fShininess, 0.0f, 0.0f, "%.2f");
    
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "EmissiveColor");
    DrawVec3Slider("EmissiveColor", m_fEmissiveColor, 0.f, 1.f);
    ImGui::InputFloat("EmissivePower", &m_fEmissivePower, 0.0f, 0.0f, "%.2f");    

    ImGui::Separator(); ImGui::Spacing();
     
    // ────────────────────────── Material Input ───────────────────────────
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Material Paths");

    ImGui::InputText("Texture", m_szTexturePath, IM_ARRAYSIZE(m_szTexturePath));
    ImGui::InputText("Shader", m_szShaderPath, IM_ARRAYSIZE(m_szShaderPath));

    ImGui::Separator(); ImGui::Spacing();

    // ───────────────────────────── Shape Info ─────────────────────────────
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shape Configuration");
    ImGui::Checkbox("Use Shape##Obj", &m_ShapeData.isUse);

    static std::vector<std::string> shapeNames = { "Box", "Sphere" };
    static std::vector<const char*> shapeNamePtrs;
    if (shapeNamePtrs.empty())
    {
        for (auto& name : shapeNames)
            shapeNamePtrs.push_back(name.c_str());
    }

    static int iSelectedShapeIndex = 0;
    ImGui::Combo("Shape Type##Obj", &iSelectedShapeIndex, shapeNamePtrs.data(), (int)shapeNamePtrs.size());
    m_ShapeData.eShapeType = (iSelectedShapeIndex == 0) ? ShapeType::ST_BOX : ShapeType::ST_SPHERE;

    DrawVec3Slider("Shape Position##Obj", m_ShapeData.Position, -100.f, 100.f);
    DrawVec3Slider("Shape Rotation##Obj", m_ShapeData.Rotation, -360.f, 360.f);
    DrawVec3Slider("Shape Scale##Obj", m_ShapeData.Scale, 0.01f, 10.f);

    ImGui::Separator(); ImGui::Spacing();

    if (ImGui::Button("Create", ImVec2(-1, 0)) && m_OnCreate)
    {
        m_OnCreate(
            m_szTexturePath,
            m_szShaderPath,
            m_szObjPath,
            Vec3(m_fPosition[0], m_fPosition[1], m_fPosition[2]),
            Vec3(m_fRotation[0], m_fRotation[1], m_fRotation[2]),
            Vec3(m_fScale[0], m_fScale[1], m_fScale[2]),
            Vec3(m_fSpecularColor[0], m_fSpecularColor[1], m_fSpecularColor[2]),
            m_fShininess,
            Vec3(m_fEmissiveColor[0], m_fEmissiveColor[1], m_fEmissiveColor[2]),
            m_fEmissivePower,
            m_ShapeData
        );
    }

    ImGui::Separator(); ImGui::Spacing();

    // ───────────────────────────── Prefab Save/Load ─────────────────────────────
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Prefab Tools");

    static char prefabName[64] = "MyObject";
    ImGui::InputText("Prefab Name", prefabName, IM_ARRAYSIZE(prefabName));

    if (ImGui::Button("Save as Prefab", ImVec2(-1, 0)))
    {
        PrefabObjectData data;
        data.Name = prefabName;
        data.MeshPath = m_szObjPath;
        data.ShaderPath = m_szShaderPath;
        data.TexturePath = m_szTexturePath;
        data.Scale = Vec3(m_fScale[0], m_fScale[1], m_fScale[2]);
        data.Rotation = Vec3(m_fRotation[0], m_fRotation[1], m_fRotation[2]);
        data.Position = Vec3(m_fPosition[0], m_fPosition[1], m_fPosition[2]);
        data.SpecularColor = Vec3(m_fSpecularColor[0], m_fSpecularColor[1], m_fSpecularColor[2]);
        data.Shininess = m_fShininess;
        data.EmissiveColor = Vec3(m_fEmissiveColor[0], m_fEmissiveColor[1], m_fEmissiveColor[2]);
        data.EmissivePower = m_fEmissivePower;
        data.ShapeData = m_ShapeData;

        PrefabLoader().SaveObject(data, "../Resources/Prefab/" + data.Name + ".object.json");
    }

    if (ImGui::Button("Load Prefab##File", ImVec2(-1, 0)))
    {
        PrefabObjectData data;
        std::string path = "../Resources/Prefab/" + std::string(prefabName) + ".object.json";
        if (PREFAB->LoadObject(path, data))
        {
            strcpy_s(m_szObjPath, data.MeshPath.c_str());
            strcpy_s(m_szShaderPath, data.ShaderPath.c_str());
            strcpy_s(m_szTexturePath, data.TexturePath.c_str());

            m_fScale[0] = data.Scale.x;    m_fScale[1] = data.Scale.y;    m_fScale[2] = data.Scale.z;
            m_fRotation[0] = data.Rotation.x; m_fRotation[1] = data.Rotation.y; m_fRotation[2] = data.Rotation.z;
            m_fPosition[0] = data.Position.x; m_fPosition[1] = data.Position.y; m_fPosition[2] = data.Position.z;

            m_fSpecularColor[0] = data.SpecularColor.x;
            m_fSpecularColor[1] = data.SpecularColor.y;
            m_fSpecularColor[2] = data.SpecularColor.z;
            m_fShininess = data.Shininess;

            m_fEmissiveColor[0] = data.EmissiveColor.x;
            m_fEmissiveColor[1] = data.EmissiveColor.y;
            m_fEmissiveColor[2] = data.EmissiveColor.z;
            m_fEmissivePower = data.EmissivePower;

            m_ShapeData = data.ShapeData;
        }
    }

    ImGui::Separator(); ImGui::Spacing();

    // ───────────────────────────── Prefab List ─────────────────────────────
    // ───────────────────────────── Prefab List ─────────────────────────────
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Prefab Browser");

    m_vObjectPrefabList = PREFAB->GetPrefabFileNames("../Resources/Prefab/", ".object.json");
    auto multiList = PREFAB->GetPrefabFileNames("../Resources/Prefab/", ".objects.json");
    m_vObjectPrefabList.insert(m_vObjectPrefabList.end(), multiList.begin(), multiList.end());

    static std::string selectedPrefabName = "";

    for (const auto& name : m_vObjectPrefabList)
    {
        if (ImGui::Selectable(name.c_str(), selectedPrefabName == name))
            selectedPrefabName = name;
    }

    if (!selectedPrefabName.empty())
    {
        if (ImGui::Button("Load Prefab##List"))
        {
            std::string path = "../Resources/Prefab/" + selectedPrefabName + ".json";

            // 복수 프리팹 처리 (.objects.json)
            if (selectedPrefabName.find(".objects") != std::string::npos)
            {
                std::vector<PrefabObjectData> dataList;
                if (PREFAB->LoadObjectArray(path, dataList))
                {
                    for (const auto& data : dataList)
                    {
                        if (m_OnCreate)
                        {
                            m_OnCreate(
                                data.TexturePath.c_str(),
                                data.ShaderPath.c_str(),
                                data.MeshPath.c_str(),
                                data.Position,
                                data.Rotation,
                                data.Scale,
                                data.SpecularColor,
                                data.Shininess,
                                data.EmissiveColor,
                                data.EmissivePower,
                                data.ShapeData
                            );
                        }
                    }
                }
            }
            else // 단일 프리팹 처리 (.object.json)
            {
                PrefabObjectData data;
                if (PREFAB->LoadObject(path, data))
                {
                    if (m_OnCreate)
                    {
                        m_OnCreate(
                            data.TexturePath.c_str(),
                            data.ShaderPath.c_str(),
                            data.MeshPath.c_str(),
                            data.Position,
                            data.Rotation,
                            data.Scale,
                            data.SpecularColor,
                            data.Shininess,
                            data.EmissiveColor,
                            data.EmissivePower,
                            data.ShapeData
                        );
                    }
                }
            }
        }
    }

    ImGui::Separator(); ImGui::Spacing();
}

void ObjectEditorUI::StartFileBrowser(const std::string& root, const std::string& relative)
{
    std::string fullPath = root + relative;
    std::string searchPath = fullPath + "\\*";

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    std::vector<std::string> folders;
    std::vector<std::string> files;

    do
    {
        const char* name = findData.cFileName;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            continue;

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            folders.push_back(name);
        }
        else
        {
            std::string ext = name;
            size_t dot = ext.find_last_of('.');
            if (dot != std::string::npos)
                ext = ext.substr(dot + 1);

            if (ext == "obj" || ext == "asset")
                files.push_back(name);
        }

    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);

    std::sort(folders.begin(), folders.end());
    std::sort(files.begin(), files.end());

    for (const auto& folder : folders)
    {
        std::string path = relative.empty() ? folder : (relative + "/" + folder);
        std::string label = "[DIR] " + folder;

        if (ImGui::TreeNode(label.c_str()))
        {
            StartFileBrowser(root, path);
            ImGui::TreePop();
        }
    }

    for (const auto& file : files)
    {
        std::string path = relative.empty() ? file : (relative + "/" + file);
        std::string label = "- " + file;

        ImGui::Indent(20.0f);
        if (ImGui::Selectable(label.c_str()))
        {
            std::string selectedFullPath = root + path;
            strcpy_s(m_szObjPath, selectedFullPath.c_str());
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("%s", (root + path).c_str());
            ImGui::EndTooltip();
        }
        ImGui::Unindent(20.0f);
    }
}

void ObjectEditorUI::CreateAtPosition(const Vec3& pos)
{
    if (!m_OnCreate)
        return;

    m_OnCreate(
        m_szTexturePath,
        m_szShaderPath,
        m_szObjPath,
        pos,
        Vec3(m_fRotation[0], m_fRotation[1], m_fRotation[2]),
        Vec3(m_fScale[0], m_fScale[1], m_fScale[2]),
        Vec3(m_fSpecularColor[0], m_fSpecularColor[1], m_fSpecularColor[2]),
        m_fShininess,
        Vec3(m_fEmissiveColor[0], m_fEmissiveColor[1], m_fEmissiveColor[2]),
        m_fEmissivePower,
        m_ShapeData
    );
}

std::vector<std::string> ObjectEditorUI::LoadMeshFileList(const std::string& folderPath)
{
    std::vector<std::string> result;

    std::string searchPath = folderPath + "*.*";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string file = findData.cFileName;
            if (file.find(".asset") != std::string::npos || file.find(".obj") != std::string::npos)
            {
                result.push_back(file);
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }

    return result;
}

std::vector<std::string> ObjectEditorUI::LoadPrefabList(const std::string& folderPath)
{
    std::vector<std::string> result;

    std::string searchPath = folderPath + "\\*.prefab.json";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string filename = findData.cFileName;
            size_t dotPos = filename.find_last_of('.');
            if (dotPos != std::string::npos)
                filename = filename.substr(0, dotPos);

            result.push_back(filename);
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }

    return result;
}

void ObjectEditorUI::DrawVec3Slider(const char* label, float* values, float minVal, float maxVal)
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

Vec3 ObjectEditorUI::SnapToGrid(const Vec3& position, float gridSize)
{
    Vec3 snapped;
    snapped.x = std::round(position.x / gridSize) * gridSize;
    snapped.y = position.y;
    snapped.z = std::round(position.z / gridSize) * gridSize;
    return snapped;
}
