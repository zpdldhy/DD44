#pragma once
#include "APawn.h"

class ASky : public APawn
{
protected:
	ComPtr<ID3D11RasterizerState> m_pCurrentRasterizer = nullptr;

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Render() override;

private:
	

};

