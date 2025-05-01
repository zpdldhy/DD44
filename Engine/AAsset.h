#pragma once
//#include <stdio.h>
#include "FbxLoader.h"

class AAsset
{
public:
	static void Export(TFbxResource _result, string fileName);
	static TFbxResource Load(const char* fileName);
};

