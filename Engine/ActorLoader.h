#pragma once
#include "APawn.h"
#include "FbxLoader.h"

class ActorLoader
{
public:
	FbxLoader m_FbxImporter;

public:
	void ConvertFbxToAsset(string _path = "../Resources/Fbx/DeathDoor/*.fbx");
	vector<shared_ptr<APawn>> Load();

};

