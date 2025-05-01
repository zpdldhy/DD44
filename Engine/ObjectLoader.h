#pragma once
#include "AssimpLoader.h"

class ObjectLoader
{
	AssimpLoader objectLoader;
	vector<shared_ptr<APawn>> objList; 
public:
	vector<shared_ptr<APawn>> Load();
};

