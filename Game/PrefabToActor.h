#pragma once
#include "Singleton.h"

#define PToA GET_SINGLE(PrefabToActor)

class AActor;

class PrefabToActor : public Singleton<PrefabToActor>
{
public:
    void Init();
public:
    vector<shared_ptr<AActor>> LoadAllPrefabs(const std::string& extension, const std::string& directiry = "../Resources/Prefab/");

public:
    shared_ptr<class ATerrainTileActor> MakeTileActor(const string& _file);
    shared_ptr<AActor> MakeCharacter(const string& _file);
    shared_ptr<class APawn> MakeObject(const string& _file);
    vector<shared_ptr<class AActor>> MakeObjects(const string& _file);
    shared_ptr<class AUIActor> MakeUI(const string& _file);
    vector<shared_ptr<class AUIActor>> MakeUIs(const string& _file);
    vector<shared_ptr<class AParticleActor>> MakeParticleGroup(const std::string& _filePath);

private:
    void MakeLoader();

//private:
//    static unique_ptr<class ActorLoader> actorLoader;
//    static unique_ptr<class MeshLoader> meshLoader;
};

