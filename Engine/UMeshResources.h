#pragma once
#include "UObject.h"
#include "Device.h"

#define MAX_INSTANCE_COUNT 5000;

enum class MeshType
{
	M_STATIC = 0,
	M_SKINNED = 1,
};

class UMeshResources : public UObject
{
public:
	UMeshResources() = default;
	virtual ~UMeshResources() = default;

protected:
	MeshType m_Type;
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
	vector<PNCT_VERTEX> m_vVertexList;
	vector<DWORD> m_vIndexList;
	D3D11_PRIMITIVE_TOPOLOGY m_Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Instance
	static ComPtr<ID3D11Buffer> m_pInstansBuffer;
	vector<INSTANCE_VERTEX> m_vInstaceList;
	UINT m_instanceCount = 0;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

protected:
	bool CreateVertexBuffer();
	bool CreateIndexBuffer();
	bool CreateInstanceBuffer();

public:
	virtual void Create() abstract;
	virtual void Bind() abstract;
	virtual void UpdateInstanceList(vector<INSTANCE_VERTEX>& _InstanceList);


public:
	ComPtr<ID3D11Buffer> GetVertexBuffer() { return m_pVertexBuffer; }
	ComPtr<ID3D11Buffer> GetIndexBuffer() { return m_pIndexBuffer; }

	vector<PNCT_VERTEX> GetVertexList() { return m_vVertexList; }
	vector<DWORD> GetIndexList() { return m_vIndexList; }
	void SetVertexList(vector<PNCT_VERTEX> _vertexList) { m_vVertexList = _vertexList; }
	void SetIndexList(vector<DWORD> _indexList) { m_vIndexList = _indexList; }
	UINT GetVertexCount() { return static_cast<UINT>(m_vVertexList.size()); }
	UINT GetIndexCount() { return static_cast<UINT>(m_vIndexList.size()); }
	int GetType() { return static_cast<int>(m_Type); }

	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) { m_Topology = _type; }

	UINT AddInstance(); 
	UINT GetInstanceCount() { return m_instanceCount; }
};

