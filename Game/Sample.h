#pragma once
#include "IExecute.h"

class Sample : public IExecute
{
public:

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
};

