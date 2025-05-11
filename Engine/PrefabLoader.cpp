#include "pch.h"
#include "PrefabLoader.h"
#include "fstream"
#include "AActor.h"

bool PrefabLoader::Save(const PrefabData& _prefab, const std::string& _filePath)
{
    json j;
    j["Name"] = _prefab.Name;
    j["ActorType"] = _prefab.ActorType;
    j["MeshPath"] = _prefab.MeshPath;
    j["ShaderPath"] = _prefab.ShaderPath;
    j["TexturePath"] = _prefab.TexturePath;

    j["Scale"] = { _prefab.Scale.x, _prefab.Scale.y, _prefab.Scale.z };
    j["Rotation"] = { _prefab.Rotation.x, _prefab.Rotation.y, _prefab.Rotation.z };
    j["Translation"] = { _prefab.Translation.x, _prefab.Translation.y, _prefab.Translation.z };

    std::ofstream file(_filePath);
    if (!file.is_open()) return false;

    file << j.dump(4); // ¿¹»Ú°Ô Ãâ·Â
    return true;
}
bool PrefabLoader::Load(const std::string& _filePath, PrefabData& _prefab)
{
    std::ifstream file(_filePath);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    _prefab.Name = j["Name"];
    _prefab.ActorType = j["ActorType"];
    _prefab.MeshPath = j["MeshPath"];
    _prefab.ShaderPath = j["ShaderPath"];
    _prefab.TexturePath = j["TexturePath"];

    auto s = j["Scale"];
    _prefab.Scale = Vec3(s[0], s[1], s[2]);

    auto r = j["Rotation"];
    _prefab.Rotation = Vec3(r[0], r[1], r[2]);

    auto t = j["Translation"];
    _prefab.Translation = Vec3(t[0], t[1], t[2]);

    return true;
}
bool PrefabLoader::SaveScene(const std::string& _filepath, const std::vector<PrefabData>& _placedPrefabs)
{
    json j;
    for (const auto& data : _placedPrefabs)
    {
        j["Prefabs"].push_back({
            { "Name",        data.Name },
            { "ActorType",   data.ActorType },
            { "MeshPath",    data.MeshPath },
            { "ShaderPath",  data.ShaderPath },
            { "TexturePath", data.TexturePath },
            { "Scale",       { data.Scale.x, data.Scale.y, data.Scale.z } },
            { "Rotation",    { data.Rotation.x, data.Rotation.y, data.Rotation.z } },
            { "Translation", { data.Translation.x, data.Translation.y, data.Translation.z } }
            });
    }

    std::ofstream file(_filepath);
    if (!file.is_open()) return false;

    file << j.dump(4);
    return true;
}
bool PrefabLoader::LoadScene(const std::string& _filepath, std::vector<PrefabData>& _placedPrefabs)
{
    std::ifstream file(_filepath);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    for (const auto& obj : j["Prefabs"])
    {
        PrefabData data;
        data.Name = obj["Name"];
        data.ActorType = obj["ActorType"];
        data.MeshPath = obj["MeshPath"];
        data.ShaderPath = obj["ShaderPath"];
        data.TexturePath = obj["TexturePath"];

        auto s = obj["Scale"];
        auto r = obj["Rotation"];
        auto t = obj["Translation"];
        data.Scale = Vec3(s[0], s[1], s[2]);
        data.Rotation = Vec3(r[0], r[1], r[2]);
        data.Translation = Vec3(t[0], t[1], t[2]);

        _placedPrefabs.push_back(data);
    }

    return true;
}

bool PrefabLoader::SaveCharacter(const PrefabCharacterData& data, const std::string& filePath)
{
    json j;
    j["Name"] = data.Name;
    j["RootMeshPath"] = data.RootMeshPath;
    j["ShaderPath"] = data.ShaderPath;
    j["TexturePath"] = data.TexturePath;
    j["ScriptType"] = data.ScriptType;
    j["AnimIndex"] = data.AnimIndex;
    j["AnimSpeed"] = data.AnimSpeed;

    j["Scale"] = { data.Scale.x, data.Scale.y, data.Scale.z };
    j["Rotation"] = { data.Rotation.x, data.Rotation.y, data.Rotation.z };
    j["Translation"] = { data.Translation.x, data.Translation.y, data.Translation.z };

    for (const auto& child : data.ChildMeshes)
    {
        j["ChildMeshes"].push_back({
            { "MeshPath", child.MeshPath },
            { "TargetBoneIndex", child.TargetBoneIndex }
            });
    }

    std::ofstream file(filePath);
    if (!file.is_open()) return false;

    file << j.dump(4);
    return true;
}

bool PrefabLoader::LoadCharacter(const std::string& filePath, PrefabCharacterData& data)
{
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    data.Name = j["Name"];
    data.RootMeshPath = j["RootMeshPath"];
    data.ShaderPath = j["ShaderPath"];
    data.TexturePath = j["TexturePath"];
    data.ScriptType = j["ScriptType"];
    data.AnimIndex = j["AnimIndex"];
    data.AnimSpeed = j["AnimSpeed"];

    auto s = j["Scale"];
    data.Scale = Vec3(s[0], s[1], s[2]);

    auto r = j["Rotation"];
    data.Rotation = Vec3(r[0], r[1], r[2]);

    auto t = j["Translation"];
    data.Translation = Vec3(t[0], t[1], t[2]);

    if (j.contains("ChildMeshes"))
    {
        for (const auto& child : j["ChildMeshes"])
        {
            PrefabCharacterData::ChildMeshData childData;
            childData.MeshPath = child["MeshPath"];
            childData.TargetBoneIndex = child["TargetBoneIndex"];
            data.ChildMeshes.push_back(childData);
        }
    }

    return true;
}
