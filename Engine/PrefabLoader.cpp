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
