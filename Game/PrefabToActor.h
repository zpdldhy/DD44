#pragma once
#include "Singleton.h"

#define PToA GET_SINGLE(PrefabToActor)

class AActor;

class PrefabToActor : public Singleton<PrefabToActor>
{
public:
    vector<shared_ptr<AActor>> LoadAllPrefabs(const std::string& extension);

public:
    shared_ptr<class ATerrainTileActor> MakeTileActor(const string& _file);
    shared_ptr<AActor> MakeCharacter(const string& _file);
    shared_ptr<class APawn> MakeObject(const string& _file);
    vector<shared_ptr<class APawn>> MakeObjects(const string& _file);
    shared_ptr<class AUIActor> MakeUI(const string& _file);

private:
    void MakeLoader();

private:
    static unique_ptr<class ActorLoader> actorLoader;
    static unique_ptr<class MeshLoader> meshLoader;
};

