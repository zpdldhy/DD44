#pragma once
#include "IExecute.h"

class Sample : public IExecute
{
public:


public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Render() override;
	virtual void Destroy() override;
};

