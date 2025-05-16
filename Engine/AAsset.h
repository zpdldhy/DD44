#pragma once
#include "FbxLoader.h"

class APawn;

class AAsset
{
public:
	static void Export(TFbxResource _result, string fileName);
	static TFbxResource Load(const char* fileName);
	//static void ExportMesh(shared_ptr<APawn> _actor);

};

