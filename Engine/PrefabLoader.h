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

private:
	void LoadActor(json& j, PrefabCharacterData& data);
	void LoadCameraComponent(json& j, CameraComponentData& data);
	void LoadShapeComponent(json& j, ShapeComponentData& data);

public:
	std::vector<std::string> GetPrefabFileList(const std::string& directory, const std::string& extension);
	std::vector<std::string> GetPrefabFileNames(const std::string& directory, const std::string& extension);
};

