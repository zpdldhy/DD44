#include "pch.h"
#include "USkeletalMeshResources.h"

bool USkeletalMeshResources::CreateIWBuffer()
{
	if (m_vIWList.size() <= 0) { return true; }
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = static_cast<UINT>(sizeof(IW_VERTEX)) * static_cast<UINT>(m_vIWList.size());
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA pInitialData;
	ZeroMemory(&pInitialData, sizeof(pInitialData));
	pInitialData.pSysMem = &m_vIWList.at(0);
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, &pInitialData, m_pIWBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

void USkeletalMeshResources::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(CBBindPoseData);
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	//D3D11_SUBRESOURCE_DATA pInitialData;
	//ZeroMemory(&pInitialData, sizeof(pInitialData));
	//pInitialData.pSysMem = &currentAnimList;
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, NULL, m_pBindPoseCB.GetAddressOf());
	if (FAILED(hr))
	{
		return;
	}
	return;
}

void USkeletalMeshResources::Create()
{
	CreateVertexBuffer();
	if (m_vIndexList.size() > 0)
	{
		CreateIndexBuffer();
	}
	CreateIWBuffer();
	CreateConstantBuffer();
}

void USkeletalMeshResources::Bind()
{
	//CreateConstantBuffer();

	UINT Strides[2] = { sizeof(PNCT_VERTEX), sizeof(IW_VERTEX) };
	UINT Offsets[2] = { 0, 0 };
	ID3D11Buffer* pVB[2] = { m_pVertexBuffer.Get(), m_pIWBuffer.Get() };

	DC->IASetVertexBuffers(0, 2, pVB, Strides, Offsets);
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void USkeletalMeshResources::UpdateBindPoseData()
{
	DC->UpdateSubresource(m_pBindPoseCB.Get(), 0, NULL, &m_inverseBindPose, 0, 0);
	DC->VSSetConstantBuffers(9, 1, m_pBindPoseCB.GetAddressOf());
}

void USkeletalMeshResources::SetInverseBindPose(vector<Matrix> _inverseBindPose)
{
	for (int i = 0; i < _inverseBindPose.size(); i++)
	{
		m_inverseBindPose.inverseBindPose[i] = _inverseBindPose[i];
	}
}

int USkeletalMeshResources::GetBoneIndex(wstring _name)
{
	auto iter = m_vSkeletonList.find(_name);
	if (iter != m_vSkeletonList.end())
	{
		return iter->second.m_iIndex;
	}
	return -1;
}
