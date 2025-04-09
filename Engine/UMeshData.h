#pragma once
class UMeshData
{
	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;
	vector<PNCT_VERTEX> m_vertexList;
	vector<DWORD> m_indexList;
public:
};

