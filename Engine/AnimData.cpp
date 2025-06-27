#include "pch.h"
#include "AnimData.h"
#include "Device.h"

void AnimData::AddData(const vector<XMFLOAT4>& _data)
{
	m_animTexData = _data;
}

void AnimData::CreateTex(int _boneCount, int _trackCount)
{
	texWidth = _boneCount;
	texHeight = 200;
	texDepth = _trackCount * 4;

	// Texture

	D3D11_TEXTURE3D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = texWidth;
	desc.Height = texHeight;
	desc.Depth = texDepth;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = m_animTexData.data();
	initData.SysMemPitch = texWidth * sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = texWidth * texHeight * sizeof(XMFLOAT4);

	HRESULT hr = DEVICE->CreateTexture3D(&desc, &initData, m_pTex3D.GetAddressOf());
	if (FAILED(hr))
	{
		//assert(false);
	}

	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture3D.MipLevels = 1;

	hr = DEVICE->CreateShaderResourceView(m_pTex3D.Get(), &srvDesc, m_pTexSRV.GetAddressOf());
	if (FAILED(hr))
	{
		//assert(false);
	}
}

void AnimData::Bind()
{
	DC->VSSetShaderResources(3, 1, m_pTexSRV.GetAddressOf());
}

Matrix AnimData::GetMatrix(int _track, int _bone, int _frame)
{
	Matrix ret;

	for (int row = 0; row < 4; ++row)
	{
		int z = _track * 4 + row;

		size_t index = _bone
			+ _frame * texWidth
			+ z * texWidth * texHeight;

		const XMFLOAT4& rowData = m_animTexData[index];

		ret.m[row][0] = rowData.x;
		ret.m[row][1] = rowData.y;
		ret.m[row][2] = rowData.z;
		ret.m[row][3] = rowData.w;
	}

	return ret;
}
