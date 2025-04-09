#pragma once
#include "Device.h"
#include "SimpleMath.h"

class Camera
{
protected:
	ComPtr<ID3D11Buffer> m_pCameraBuffer;

	Matrix m_matView;
	Matrix m_matProjection;
	
private:
	virtual void Init();
	virtual void Tick();
	virtual void Render();
	virtual void Release();

	void CreateCameraBuffer();
};

