#pragma once
#include "APawn.h"

class ASky : public APawn
{
public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Render() override;

private:
	

};

