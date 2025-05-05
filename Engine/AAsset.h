#pragma once
#include "FbxLoader.h"

class AAsset
{
public:
	static void Export(TFbxResource _result, string fileName);
	static ParsingData Load(const char* fileName);

};

