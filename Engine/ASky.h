#pragma once
#include "AActor.h"

class ASky : public AActor
{
public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Render() override;

private:
	

};

