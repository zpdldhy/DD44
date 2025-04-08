#include "pch.h"
#include "DxState.h"
#include "Device.h"

void DxState::Create()
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	HRESULT hr = DEVICE->CreateSamplerState(&sd, &_linearSS);
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateSamplerState"));
	}
}
