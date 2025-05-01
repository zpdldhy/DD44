#pragma once
#include "UObject.h"
#include "Device.h"

class UMeshResources : public UObject
{
public:
	UMeshResources() = default;
	virtual ~UMeshResources() = default;

protected:
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
	vector<PNCT_VERTEX> m_vVertexList;
	vector<DWORD> m_vIndexList;

private:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

protected:
	bool CreateVertexBuffer();
	bool CreateIndexBuffer();

public:
	virtual void Bind() abstract;

public:
	ComPtr<ID3D11Buffer> GetVertexBuffer() { return m_pVertexBuffer; }
	ComPtr<ID3D11Buffer> GetIndexBuffer() { return m_pIndexBuffer; }

	vector<PNCT_VERTEX> GetVertexList() { return m_vVertexList; }
	vector<DWORD> GetIndexList() { return m_vIndexList; }
	void SetVertexList(vector<PNCT_VERTEX> _vertexList) { m_vVertexList = _vertexList; }
	void SetIndexList(vector<DWORD> _indexList) { m_vIndexList = _indexList; }
	UINT GetVertexCount() { return static_cast<UINT>(m_vVertexList.size()); }
	UINT GetIndexCount() { return static_cast<UINT>(m_vIndexList.size()); }
};

