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
    j["Translation"] = { _prefab.Position.x, _prefab.Position.y, _prefab.Position.z };

    std::ofstream file(_filePath);
    if (!file.is_open()) return false;

    file << j.dump(4); // 예쁘게 출력
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
    _prefab.Position = Vec3(t[0], t[1], t[2]);

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
            { "Translation", { data.Position.x, data.Position.y, data.Position.z } }
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
        data.Position = Vec3(t[0], t[1], t[2]);

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

    SaveTransform(j, data.transform);

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

    LoadTransform(j, data.transform);

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
    j["Position"] = { _prefab.Position.x, _prefab.Position.y, _prefab.Position.z };
    j["SpecularColor"] = { _prefab.SpecularColor.x,_prefab.SpecularColor.y ,_prefab.SpecularColor.z };
    j["Shininess"] = _prefab.Shininess;
    j["EmissiveColor"] = { _prefab.EmissiveColor.x,_prefab.EmissiveColor.y ,_prefab.EmissiveColor.z };
    j["EmissivePower"] = _prefab.EmissivePower;

    j["ShapeData"] = {
        { "isUse", _prefab.ShapeData.isUse },
        { "eShapeType", _prefab.ShapeData.eShapeType },
        { "Scale", { _prefab.ShapeData.Scale[0], _prefab.ShapeData.Scale[1], _prefab.ShapeData.Scale[2] } },
        { "Position", { _prefab.ShapeData.Position[0], _prefab.ShapeData.Position[1], _prefab.ShapeData.Position[2] } },
        { "Rotation", { _prefab.ShapeData.Rotation[0], _prefab.ShapeData.Rotation[1], _prefab.ShapeData.Rotation[2] } }
    };

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

    auto t = j["Position"];
    _prefab.Position = Vec3(t[0], t[1], t[2]);

    auto sc = j["SpecularColor"];
    _prefab.SpecularColor = Vec3(sc[0], sc[1], sc[2]);
    _prefab.Shininess = j["Shininess"];

    auto ec = j["EmissiveColor"];
    _prefab.EmissiveColor = Vec3(ec[0], ec[1], ec[2]);
    _prefab.EmissivePower = j["EmissivePower"];

    if (j.contains("ShapeData"))
    {
        auto shape = j["ShapeData"];
        _prefab.ShapeData.isUse = shape["isUse"];
        _prefab.ShapeData.eShapeType = shape["eShapeType"];

        auto sca = shape["Scale"];
        _prefab.ShapeData.Scale[0] = sca[0];
        _prefab.ShapeData.Scale[1] = sca[1];
        _prefab.ShapeData.Scale[2] = sca[2];

        auto pos = shape["Position"];
        _prefab.ShapeData.Position[0] = pos[0];
        _prefab.ShapeData.Position[1] = pos[1];
        _prefab.ShapeData.Position[2] = pos[2];

        auto rot = shape["Rotation"];
        _prefab.ShapeData.Rotation[0] = rot[0];
        _prefab.ShapeData.Rotation[1] = rot[1];
        _prefab.ShapeData.Rotation[2] = rot[2];
    }

    return true;
}

bool PrefabLoader::SaveObjectArray(const std::vector<PrefabObjectData>& _prefabs, const std::string& _filePath)
{
    json j = json::array();

    for (const auto& p : _prefabs)
    {
        json item;
        item["Name"] = p.Name;
        item["MeshPath"] = p.MeshPath;
        item["ShaderPath"] = p.ShaderPath;
        item["TexturePath"] = p.TexturePath;

        item["Scale"] = { p.Scale.x, p.Scale.y, p.Scale.z };
        item["Rotation"] = { p.Rotation.x, p.Rotation.y, p.Rotation.z };
        item["Position"] = { p.Position.x, p.Position.y, p.Position.z };

        item["SpecularColor"] = { p.SpecularColor.x, p.SpecularColor.y, p.SpecularColor.z };
        item["Shininess"] = p.Shininess;

        item["EmissiveColor"] = { p.EmissiveColor.x, p.EmissiveColor.y, p.EmissiveColor.z };
        item["EmissivePower"] = p.EmissivePower;

        if (p.ShapeData.isUse)
        {
            json shape;
            shape["isUse"] = p.ShapeData.isUse;
            shape["eShapeType"] = p.ShapeData.eShapeType;

            shape["Scale"] = {
                p.ShapeData.Scale[0], p.ShapeData.Scale[1], p.ShapeData.Scale[2]
            };
            shape["Position"] = {
                p.ShapeData.Position[0], p.ShapeData.Position[1], p.ShapeData.Position[2]
            };
            shape["Rotation"] = {
                p.ShapeData.Rotation[0], p.ShapeData.Rotation[1], p.ShapeData.Rotation[2]
            };

            item["ShapeData"] = shape;
        }

        j.push_back(item);
    }

    std::ofstream file(_filePath);
    if (!file.is_open()) return false;

    file << j.dump(4);
    return true;
}

bool PrefabLoader::LoadObjectArray(const std::string& _filePath, std::vector<PrefabObjectData>& _outPrefabs)
{
    std::ifstream file(_filePath);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    if (!j.is_array()) return false;

    for (auto& item : j)
    {
        PrefabObjectData _prefab;
        _prefab.Name = item["Name"];
        _prefab.MeshPath = item["MeshPath"];
        _prefab.ShaderPath = item["ShaderPath"];
        _prefab.TexturePath = item["TexturePath"];

        auto s = item["Scale"];
        _prefab.Scale = Vec3(s[0], s[1], s[2]);

        auto r = item["Rotation"];
        _prefab.Rotation = Vec3(r[0], r[1], r[2]);

        auto t = item["Position"];
        _prefab.Position = Vec3(t[0], t[1], t[2]);

        auto sc = item["SpecularColor"];
        _prefab.SpecularColor = Vec3(sc[0], sc[1], sc[2]);
        _prefab.Shininess = item["Shininess"];

        auto ec = item["EmissiveColor"];
        _prefab.EmissiveColor = Vec3(ec[0], ec[1], ec[2]);
        _prefab.EmissivePower = item["EmissivePower"];

        if (item.contains("ShapeData"))
        {
            auto shape = item["ShapeData"];

            _prefab.ShapeData.isUse = shape["isUse"];
            _prefab.ShapeData.eShapeType = shape["eShapeType"];

            auto sca = shape["Scale"];
            _prefab.ShapeData.Scale[0] = sca[0];
            _prefab.ShapeData.Scale[1] = sca[1];
            _prefab.ShapeData.Scale[2] = sca[2];

            auto pos = shape["Position"];
            _prefab.ShapeData.Position[0] = pos[0];
            _prefab.ShapeData.Position[1] = pos[1];
            _prefab.ShapeData.Position[2] = pos[2];

            auto rot = shape["Rotation"];
            _prefab.ShapeData.Rotation[0] = rot[0];
            _prefab.ShapeData.Rotation[1] = rot[1];
            _prefab.ShapeData.Rotation[2] = rot[2];
        }

        _outPrefabs.push_back(_prefab);
    }

    return true;
}

bool PrefabLoader::SaveUI(const PrefabUIData& _prefab, const std::string& _filePath)
{
    json j;
    j["Name"] = _prefab.Name;

    // position은 NDC로 변환해서 저장    
	auto transform = _prefab.transform;
    transform.Position[0] = transform.Position[0] / (static_cast<float>(g_windowSize.x) / 2.0f);
	transform.Position[1] = transform.Position[1] / (static_cast<float>(g_windowSize.y) / 2.0f);

    SaveTransform(j, transform);

    j["Color"] = { _prefab.color[0], _prefab.color[1] ,_prefab.color[2] ,_prefab.color[3] };
    j["SliceUV"] = { _prefab.SliceUV[0], _prefab.SliceUV[1], _prefab.SliceUV[2], _prefab.SliceUV[3] };

    j["TexturePath"] = {
       { "Idle", _prefab.IdleTexturePath },
       { "Hover", _prefab.HoverTexturePath },
       { "Active", _prefab.ActiveTexturePath },
       { "Selected", _prefab.SelectedTexturePath }
    };

    j["ShaderPath"] = _prefab.ShaderPath;

    std::ofstream file(_filePath);
    if (!file.is_open()) return false;

    file << j.dump(4);
    return true;
}

bool PrefabLoader::LoadUI(const std::string& _filePath, PrefabUIData& _prefab)
{
    std::ifstream file(_filePath);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    _prefab.Name = j["Name"];

    // position은 NDC로 변환해서 로드

    LoadTransform(j, _prefab.transform);

	_prefab.transform.Position[0] *= (static_cast<float>(g_windowSize.x) / 2.0f);
	_prefab.transform.Position[1] *= (static_cast<float>(g_windowSize.y) / 2.0f);

    auto c = j["Color"];
    for (int i = 0; i < 4; i++)   _prefab.color[i] = c[i];

    auto s = j["SliceUV"];
    for (int i = 0; i < 4; i++)   _prefab.SliceUV[i] = s[i];

    _prefab.IdleTexturePath = j["TexturePath"]["Idle"];
    _prefab.HoverTexturePath = j["TexturePath"]["Hover"];
    _prefab.ActiveTexturePath = j["TexturePath"]["Active"];
    _prefab.SelectedTexturePath = j["TexturePath"]["Selected"];
    _prefab.ShaderPath = j["ShaderPath"];

    return true;
}

bool PrefabLoader::SaveUIs(const vector<PrefabUIData>& _prefabs, const string& _filePath)
{
    json j = json::array();

    for (const auto& p : _prefabs)
    {
        json item;
        item["Name"] = p.Name;

        // position은 NDC로 변환해서 저장    
        auto transform = p.transform;
        transform.Position[0] = transform.Position[0] / (static_cast<float>(g_windowSize.x) / 2.0f);
        transform.Position[1] = transform.Position[1] / (static_cast<float>(g_windowSize.y) / 2.0f);

        SaveTransform(item, transform);

        item["Color"] = { p.color[0], p.color[1] ,p.color[2] ,p.color[3] };
        item["SliceUV"] = { p.SliceUV[0], p.SliceUV[1], p.SliceUV[2], p.SliceUV[3] };

        item["TexturePath"] = {
           { "Idle", p.IdleTexturePath },
           { "Hover", p.HoverTexturePath },
           { "Active", p.ActiveTexturePath },
           { "Selected", p.SelectedTexturePath }
        };

        item["ShaderPath"] = p.ShaderPath;

        j.push_back(item);
    }

    std::ofstream file(_filePath);
    if (!file.is_open()) return false;

    file << j.dump(4);
    return true;
}

bool PrefabLoader::LoadUIs(const string& _filePath, vector<PrefabUIData>& _outPrefabs)
{
    return false;
}

bool PrefabLoader::SaveParticle(const PrefabParticleData& data, const std::string& filePath)
{
    json j;

    j["Name"] = data.Name;
    j["ShaderPath"] = data.ShaderPath;
    j["TexturePath"] = data.TexturePath;

    j["Scale"] = { data.Scale.x, data.Scale.y, data.Scale.z };
    j["Rotation"] = { data.Rotation.x, data.Rotation.y, data.Rotation.z };
    j["Translation"] = { data.Translation.x, data.Translation.y, data.Translation.z };

    j["Divisions"] = data.Divisions;
    j["Row"] = data.Row;
    j["Col"] = data.Col;

    j["UVStart"] = { data.UVStart.x, data.UVStart.y };
    j["UVEnd"] = { data.UVEnd.x, data.UVEnd.y };

    j["BillboardSizeX"] = data.BillboardSizeX;
    j["BillboardSizeY"] = data.BillboardSizeY;

    j["Duration"] = data.Duration;
    j["bLoop"] = data.bLoop;
    j["bAutoDestroy"] = data.bAutoDestroy;

    std::ofstream file(filePath);
    if (!file.is_open())
        return false;

    file << std::setw(4) << j;
    return true;
}

bool PrefabLoader::LoadParticle(PrefabParticleData& outData, const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        return false;

    nlohmann::json j;
    file >> j;

    outData.Name = j["Name"];
    outData.ShaderPath = j["ShaderPath"];
    outData.TexturePath = j["TexturePath"];

    // 배열 파싱
    auto t = j["Translation"];
    outData.Translation = Vec3(t[0], t[1], t[2]);

    auto r = j["Rotation"];
    outData.Rotation = Vec3(r[0], r[1], r[2]);

    auto s = j["Scale"];
    outData.Scale = Vec3(s[0], s[1], s[2]);

    auto uvStart = j["UVStart"];
    auto uvEnd = j["UVEnd"];
    outData.UVStart = Vec2(uvStart[0], uvStart[1]);
    outData.UVEnd = Vec2(uvEnd[0], uvEnd[1]);

    // 숫자
    outData.Divisions = j["Divisions"];
    outData.Row = j["Row"];
    outData.Col = j["Col"];
    outData.BillboardSizeX = j["BillboardSizeX"];
    outData.BillboardSizeY = j["BillboardSizeY"];
    outData.Duration = j.value("Duration", 1.0f);
    outData.bLoop = j.value("bLoop", true);
    outData.bAutoDestroy = j.value("bAutoDestroy", false);



    return true;
}

bool PrefabLoader::SaveParticleGroup(const PrefabParticleGroupData& data, const std::string& path)
{
    nlohmann::json j;
    j["GroupName"] = data.GroupName;

    for (const auto& particle : data.Particles)
    {
        nlohmann::json p;
        p["Name"] = particle.Name;
        p["ShaderPath"] = particle.ShaderPath;
        p["TexturePath"] = particle.TexturePath;

        p["Scale"] = { particle.Scale.x, particle.Scale.y, particle.Scale.z };
        p["Rotation"] = { particle.Rotation.x, particle.Rotation.y, particle.Rotation.z };
        p["Translation"] = { particle.Translation.x, particle.Translation.y, particle.Translation.z };

        p["Divisions"] = particle.Divisions;
        p["Row"] = particle.Row;
        p["Col"] = particle.Col;

        p["UVStart"] = { particle.UVStart.x, particle.UVStart.y };
        p["UVEnd"] = { particle.UVEnd.x, particle.UVEnd.y };

        p["BillboardSizeX"] = particle.BillboardSizeX;
        p["BillboardSizeY"] = particle.BillboardSizeY;

        p["Duration"] = particle.Duration;
        p["bLoop"] = particle.bLoop;
        p["bAutoDestroy"] = particle.bAutoDestroy;

        j["Particles"].push_back(p);
    }

    std::ofstream file(path);
    if (!file.is_open()) return false;

    file << j.dump(4); // 예쁘게 저장
    return true;
}

bool PrefabLoader::LoadParticleGroup(PrefabParticleGroupData& out, const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) return false;

    nlohmann::json j;
    file >> j;

    out.GroupName = j.value("GroupName", "");

    const auto& jParticles = j["Particles"];
    for (const auto& p : jParticles)
    {
        PrefabParticleData particle;

        particle.Name = p.value("Name", "");
        particle.ShaderPath = p.value("ShaderPath", "");
        particle.TexturePath = p.value("TexturePath", "");

        auto s = p["Scale"];
        particle.Scale = Vec3(s[0], s[1], s[2]);

        auto r = p["Rotation"];
        particle.Rotation = Vec3(r[0], r[1], r[2]);

        auto t = p["Translation"];
        particle.Translation = Vec3(t[0], t[1], t[2]);

        particle.Divisions = p.value("Divisions", 4);
        particle.Row = p.value("Row", 0);
        particle.Col = p.value("Col", 0);

        auto uv0 = p["UVStart"];
        auto uv1 = p["UVEnd"];
        particle.UVStart = Vec2(uv0[0], uv0[1]);
        particle.UVEnd = Vec2(uv1[0], uv1[1]);

        particle.BillboardSizeX = p.value("BillboardSizeX", 100.0f);
        particle.BillboardSizeY = p.value("BillboardSizeY", 100.0f);

        particle.Duration = p.value("Duration", 1.0f);
        particle.bLoop = p.value("bLoop", true);
        particle.bAutoDestroy = p.value("bAutoDestroy", false);

        out.Particles.push_back(particle);
    }

    return true;
}

bool PrefabLoader::DeletePrefab(const std::string& _filePath)
{
    if (std::remove(_filePath.c_str()) == 0) {
        return true; // 성공적으로 삭제됨
    }
    else {
        perror("파일 삭제 실패");
        return false;
    }
}


void PrefabLoader::SaveTransform(json& j, const TransformData& data)
{
    j["Transform"] = {
    { "Scale" , { data.Scale[0], data.Scale[1], data.Scale[2] }},
    { "Rotation" , { data.Rotation[0], data.Rotation[1], data.Rotation[2] }},
    { "Translation" , { data.Position[0], data.Position[1], data.Position[2] }}
    };
}

void PrefabLoader::LoadTransform(json& j, TransformData& data)
{
    auto s = j["Transform"]["Scale"];
    for (int i = 0; i < 3; i++) data.Scale[i] = s[i];

    auto r = j["Transform"]["Rotation"];
    for (int i = 0; i < 3; i++) data.Rotation[i] = r[i];

    auto t = j["Transform"]["Translation"];
    for (int i = 0; i < 3; i++) data.Position[i] = t[i];
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