#pragma once
#include "Singleton.h"
class DxState : public Singleton<DxState>
{
public:
	ComPtr<ID3D11SamplerState> _linearSS;
public:
	void Create();
};

