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
    j["MeshPath"] = data.MeshPath;
    j["ShaderPath"] = data.ShaderPath;
    j["TexturePath"] = data.TexturePath;
    j["ScriptType"] = data.ScriptType;
    j["AnimIndex"] = data.AnimIndex;
    j["AnimSpeed"] = data.AnimSpeed;

    j["Actor"] = {
        { "Scale" , { data.actor.Scale[0], data.actor.Scale[1], data.actor.Scale[2] }},
		{ "Rotation" , { data.actor.Rotation[0], data.actor.Rotation[1], data.actor.Rotation[2] }},
		{ "Translation" , { data.actor.Position[0], data.actor.Position[1], data.actor.Position[2] }}
    };

	j["CameraComponent"] = {
		{ "isUse", data.camera.isUse },
		{ "Position", { data.camera.Position[0], data.camera.Position[1], data.camera.Position[2] } },
		{ "Rotation", { data.camera.Rotation[0], data.camera.Rotation[1], data.camera.Rotation[2] } },
		{ "Fov", data.camera.Fov },
		{ "Aspect", data.camera.Aspect },
		{ "Near", data.camera.Near },
		{ "Far", data.camera.Far }
	};

	j["ShapeComponent"] = {
		{ "isUse", data.shape.isUse },
		{ "ShapeType", static_cast<UINT>(data.shape.eShapeType) },
		{ "Position", { data.shape.Position[0], data.shape.Position[1], data.shape.Position[2] } },
		{ "Rotation", { data.shape.Rotation[0], data.shape.Rotation[1], data.shape.Rotation[2] } },
		{ "Scale", { data.shape.Scale[0], data.shape.Scale[1], data.shape.Scale[2] } }
	};

    //for (const auto& child : data.ChildMeshes)
    //{
    //    j["ChildMeshes"].push_back({
    //        { "MeshPath", child.MeshPath },
    //        { "TargetBoneIndex", child.TargetBoneIndex }
    //        });
    //}

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
    data.MeshPath = j["MeshPath"];
    data.ShaderPath = j["ShaderPath"];
    data.TexturePath = j["TexturePath"];
    data.ScriptType = j["ScriptType"];
    data.AnimIndex = j["AnimIndex"];
    data.AnimSpeed = j["AnimSpeed"];

    LoadActor(j, data);

    // Component
	LoadCameraComponent(j, data.camera);
	LoadShapeComponent(j, data.shape);

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

bool PrefabLoader::SaveMapTile(const PrefabMapData& data, const std::string& filePath)
{
    json j;
    j["Cols"] = data.Cols;
    j["Rows"] = data.Rows;
    j["CellSize"] = data.CellSize;
    j["Position"] = { data.Position.x, data.Position.y, data.Position.z };
    j["Rotation"] = { data.Rotation.x, data.Rotation.y, data.Rotation.z };
    j["Scale"] = { data.Scale.x, data.Scale.y, data.Scale.z };
    j["SelectedRow"] = data.SelectedRow;
    j["SelectedCol"] = data.SelectedCol;
    j["TargetHeight"] = data.TargetHeight;
    j["TexturePath"] = data.TexturePath;
    j["ShaderPath"] = data.ShaderPath;

    std::ofstream out(filePath);
    if (!out.is_open()) return false;
    out << j.dump(4);
    return true;
}

bool PrefabLoader::LoadMapTile(const std::string& filePath, PrefabMapData& data)
{
    std::ifstream in(filePath);
    if (!in.is_open()) return false;

    json j;
    in >> j;

    data.Cols = j["Cols"];
    data.Rows = j["Rows"];
    data.CellSize = j["CellSize"];
    auto pos = j["Position"];
    auto rot = j["Rotation"];
    auto scale = j["Scale"];
    data.Position = Vec3(pos[0], pos[1], pos[2]);
    data.Rotation = Vec3(rot[0], rot[1], rot[2]);
    data.Scale = Vec3(scale[0], scale[1], scale[2]);
    data.SelectedRow = j.value("SelectedRow", -1);
    data.SelectedCol = j.value("SelectedCol", -1);
    data.TargetHeight = j.value("TargetHeight", 0.0f);
    data.TexturePath = j["TexturePath"];
    data.ShaderPath = j["ShaderPath"];

    return true;
}


bool PrefabLoader::SaveObject(const PrefabObjectData& _prefab, const std::string& _filePath)
{
    json j;
    j["Name"] = _prefab.Name;
    j["MeshPath"] = _prefab.MeshPath;
    j["ShaderPath"] = _prefab.ShaderPath;
    j["TexturePath"] = _prefab.TexturePath;
    j["Scale"] = { _prefab.Scale.x, _prefab.Scale.y, _prefab.Scale.z };
    j["Rotation"] = { _prefab.Rotation.x, _prefab.Rotation.y, _prefab.Rotation.z };
    j["Translation"] = { _prefab.Translation.x, _prefab.Translation.y, _prefab.Translation.z };
    j["SpecularColor"] = { _prefab.SpecularColor.x,_prefab.SpecularColor.y ,_prefab.SpecularColor.z };
    j["Shininess"] = _prefab.Shininess;
    j["EmissiveColor"] = { _prefab.EmissiveColor.x,_prefab.EmissiveColor.y ,_prefab.EmissiveColor.z };
    j["EmissivePower"] = _prefab.EmissivePower;

    std::ofstream file(_filePath);
    if (!file.is_open()) return false;

    file << j.dump(4);
    return true;
}

bool PrefabLoader::LoadObject(const std::string& _filePath, PrefabObjectData& _prefab)
{
    std::ifstream file(_filePath);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    _prefab.Name = j["Name"];
    _prefab.MeshPath = j["MeshPath"];
    _prefab.ShaderPath = j["ShaderPath"];
    _prefab.TexturePath = j["TexturePath"];

    auto s = j["Scale"];
    _prefab.Scale = Vec3(s[0], s[1], s[2]);

    auto r = j["Rotation"];
    _prefab.Rotation = Vec3(r[0], r[1], r[2]);

    auto t = j["Translation"];
    _prefab.Translation = Vec3(t[0], t[1], t[2]);

    auto sc = j["SpecularColor"];
    _prefab.SpecularColor = Vec3(sc[0], sc[1], sc[2]);
    _prefab.Shininess = j["Shininess"];

    auto ec = j["EmissiveColor"];
    _prefab.EmissiveColor = Vec3(ec[0], ec[1], ec[2]);
    _prefab.EmissivePower = j["EmissivePower"];

    return true;
}

void PrefabLoader::LoadActor(json& j, PrefabCharacterData& data)
{
    auto s = j["Actor"]["Scale"];
    for (int i = 0; i < 3; i++) data.actor.Scale[i] = s[i];

    auto r = j["Actor"]["Rotation"];
    for (int i = 0; i < 3; i++) data.actor.Rotation[i] = r[i];

    auto t = j["Actor"]["Translation"];
    for (int i = 0; i < 3; i++) data.actor.Position[i] = t[i];
}

void PrefabLoader::LoadCameraComponent(json& j, CameraComponentData& data)
{
	data.isUse = j["CameraComponent"]["isUse"];
	auto pos = j["CameraComponent"]["Position"];
	auto rot = j["CameraComponent"]["Rotation"];
	data.Fov = j["CameraComponent"]["Fov"];
	data.Aspect = j["CameraComponent"]["Aspect"];
	data.Near = j["CameraComponent"]["Near"];
	data.Far = j["CameraComponent"]["Far"];
	for (int i = 0; i < 3; i++) data.Position[i] = pos[i];
	for (int i = 0; i < 3; i++) data.Rotation[i] = rot[i];
}

void PrefabLoader::LoadShapeComponent(json& j, ShapeComponentData& data)
{
	data.isUse = j["ShapeComponent"]["isUse"];
	data.eShapeType = static_cast<ShapeType>(j["ShapeComponent"]["ShapeType"]);
	auto pos = j["ShapeComponent"]["Position"];
	auto rot = j["ShapeComponent"]["Rotation"];
	auto scale = j["ShapeComponent"]["Scale"];
	for (int i = 0; i < 3; i++) data.Position[i] = pos[i];
	for (int i = 0; i < 3; i++) data.Rotation[i] = rot[i];
	for (int i = 0; i < 3; i++) data.Scale[i] = scale[i];
}

std::vector<std::string> PrefabLoader::GetPrefabFileList(const std::string& directory, const std::string& extension)
{
    std::vector<std::string> files;
    std::string searchPath = directory + "*" + extension;

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                files.push_back(directory + findData.cFileName);
            }
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
    }

    return files;
}

std::vector<std::string> PrefabLoader::GetPrefabFileNames(const std::string& directory, const std::string& extension)
{
    std::vector<std::string> result;
    auto files = GetPrefabFileList(directory, extension);

    for (auto& filePath : files)
    {
        size_t lastSlash = filePath.find_last_of("/\\");
        std::string fileName = (lastSlash == std::string::npos) ? filePath : filePath.substr(lastSlash + 1);
        size_t dotPos = fileName.find_last_of('.');
        if (dotPos != std::string::npos)
            fileName = fileName.substr(0, dotPos);

        result.push_back(fileName);
    }

    return result;
}

