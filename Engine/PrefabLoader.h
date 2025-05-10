#pragma once
#include "Singleton.h"

struct PrefabData
{
	std::string Name;
	std::string ActorType;

	std::string MeshPath;
	std::string ShaderPath;
	std::string TexturePath;

	Vec3 Scale;
	Vec3 Rotation;
	Vec3 Translation;
};

struct CharacterPrefabData
{
    std::string Name;

    std::string RootMeshPath;
    std::string ShaderPath;
    std::string TexturePath;

    int ScriptType;

    int AnimIndex;
    float AnimSpeed;

    Vec3 Scale;
    Vec3 Rotation;
    Vec3 Translation;

    struct ChildMeshData
    {
        std::string MeshPath;
        int TargetBoneIndex;
    };
    std::vector<ChildMeshData> ChildMeshes;
};

class PrefabLoader : public Singleton<PrefabLoader>
{
public:
	bool Save(const PrefabData& _prefab, const std::string& _filePath);
	bool Load(const std::string& _filePath, PrefabData& _prefab);
	
	bool SaveScene(const std::string& _filepath, const std::vector<PrefabData>& _placedPrefabs);
	bool LoadScene(const std::string& _filepath, std::vector<PrefabData>& _placedPrefabs);

    bool SaveCharacter(const CharacterPrefabData& _data, const std::string& _filePath);
    bool LoadCharacter(const std::string& _filePath, CharacterPrefabData& _data);
};

