#pragma once
#include "Singleton.h"
#include "PrefabData.h"

class PrefabLoader : public Singleton<PrefabLoader>
{
public:
    bool Save(const PrefabData& _prefab, const std::string& _filePath);
    bool Load(const std::string& _filePath, PrefabData& _prefab);

    bool SaveScene(const std::string& _filepath, const std::vector<PrefabData>& _placedPrefabs);
    bool LoadScene(const std::string& _filepath, std::vector<PrefabData>& _placedPrefabs);

    bool SaveCharacter(const PrefabCharacterData& _data, const std::string& _filePath);
    bool LoadCharacter(const std::string& _filePath, PrefabCharacterData& _data);

    bool SaveMapTile(const PrefabMapData& data, const std::string& filePath);
    bool LoadMapTile(const std::string& filePath, PrefabMapData& data);

    bool SaveObject(const PrefabObjectData& _prefab, const std::string& _filePath);
    bool LoadObject(const std::string& _filePath, PrefabObjectData& _prefab);
    bool SaveObjectArray(const std::vector<PrefabObjectData>& _prefabs, const std::string& _filePath);
    bool LoadObjectArray(const std::string& _filePath, std::vector<PrefabObjectData>& _outPrefabs);

    bool SaveUI(const PrefabUIData& _prefab, const std::string& _filePath);
    bool LoadUI(const std::string& _filePath, PrefabUIData& _prefab);

    //particle 개별
    bool SaveParticle(const PrefabParticleData& data, const std::string& filePath);
    bool LoadParticle(PrefabParticleData& outData, const std::string& filePath);
    //particle 그룹
    bool SaveParticleGroup(const PrefabParticleGroupData& data, const std::string& path);
    bool LoadParticleGroup(PrefabParticleGroupData& out, const std::string& path);

    bool DeletePrefab(const std::string& _filePath);

private:
    void SaveTransform(json& j, const TransformData& data);

    void LoadTransform(json& j, TransformData& data);
    void LoadCameraComponent(json& j, CameraComponentData& data);
    void LoadShapeComponent(json& j, ShapeComponentData& data);

public:
    std::vector<std::string> GetPrefabFileList(const std::string& directory, const std::string& extension);
    std::vector<std::string> GetPrefabFileNames(const std::string& directory, const std::string& extension);
};

