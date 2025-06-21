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

bool USkeletalMeshResources::CreateIWInstanceBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = static_cast<UINT>(sizeof(ANIM_VERTEX)) * MAX_INSTANCE_COUNT;
	pDesc.Usage = D3D11_USAGE_DYNAMIC;
	pDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	pDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = DEVICE->CreateBuffer(&pDesc, NULL, m_pIWInstanceBuffer.GetAddressOf());
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

void USkeletalMeshResources::Init()
{
	m_vInstaceList.resize(m_instanceCount);
	m_vIWInstanceList.resize(m_instanceCount);

	CreateIWInstanceBuffer();
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
	UINT Strides[4] = { sizeof(PNCT_VERTEX), sizeof(IW_VERTEX), sizeof(INSTANCE_VERTEX), sizeof(ANIM_VERTEX) };
	UINT Offsets[4] = { 0, 0, 0, 0 };
	ID3D11Buffer* pVB[4] = { m_pVertexBuffer.Get(), m_pIWBuffer.Get(), m_pInstansBuffer.Get(), m_pIWInstanceBuffer.Get() };

	DC->IASetVertexBuffers(0, 4, pVB, Strides, Offsets);
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void USkeletalMeshResources::UpdateBindPoseData()
{
	DC->UpdateSubresource(m_pBindPoseCB.Get(), 0, NULL, &m_inverseBindPose, 0, 0);
	DC->VSSetConstantBuffers(9, 1, m_pBindPoseCB.GetAddressOf());
}

void USkeletalMeshResources::UpdateInstanceData()
{
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (SUCCEEDED(DC->Map(m_pInstansBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			memcpy(mappedResource.pData, m_vInstaceList.data(), sizeof(INSTANCE_VERTEX) * m_vInstaceList.size());
			DC->Unmap(m_pInstansBuffer.Get(), 0);
		}
	}

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (SUCCEEDED(DC->Map(m_pIWInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			memcpy(mappedResource.pData, m_vIWInstanceList.data(), sizeof(ANIM_VERTEX) * m_vIWInstanceList.size());
			DC->Unmap(m_pIWInstanceBuffer.Get(), 0);
		}
	}
}

void USkeletalMeshResources::AddInstanceData(UINT _index, INSTANCE_VERTEX _transform, ANIM_VERTEX _anim)
{
	if (_index >= m_instanceCount)
	{
		assert(false);
	}
	m_vInstaceList[_index] = _transform;
	m_vIWInstanceList[_index] = _anim;
}

UINT USkeletalMeshResources::AllocateInstanceIndex()
{
	UINT index = 0;
	if (!m_freeIndices.empty())
	{
		index = m_freeIndices.front();
		m_freeIndices.pop();
	}
	else
	{
		// 여길 들어올 일이 없어야된느거 아닌가?
		index = m_instanceCount++;
		m_vInstaceList.resize(m_instanceCount);
		m_vIWInstanceList.resize(m_instanceCount);
	}

	return index;
}

void USkeletalMeshResources::FreeInstanceIndex(UINT index)
{
	if (index < m_vInstaceList.size())
	{
		m_freeIndices.push(index);
		m_instanceCount--;
	}
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
