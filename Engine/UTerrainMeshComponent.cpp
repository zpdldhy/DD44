#include "pch.h"
#include "UTerrainMeshComponent.h"
#include "UMaterial.h"

void UTerrainMeshComponent::Render()
{
	PreRender();

	//if (m_pWorldCB)
	//{
	//	DC->UpdateSubresource(m_pWorldCB.Get(), 0, NULL, &m_cbData, 0, 0);
	//	DC->VSSetConstantBuffers(0, 1, m_pWorldCB.GetAddressOf());
	//}

	PostRender();
}

void UTerrainMeshComponent::PreRender()
{
	if (m_pMesh)
	{
		m_pMesh->Bind();
	}
	if (m_pMaterial)
	{
		m_pMaterial->Bind();
	}
}

void UTerrainMeshComponent::PostRender()
{
	if (m_pMesh->GetIndexCount() <= 0)
		DC->Draw(m_pMesh->GetVertexCount(), 0);
	else
		DC->DrawIndexed(m_pMesh->GetIndexCount(), 0, 0);
}

void UTerrainMeshComponent::Destroy()
{
}

void UTerrainMeshComponent::CreateGrid(int _sizeX, int _sizeZ, float _cellSize)
{
	if (m_pMesh)
	{
		return;
	}

	vector<PNCT_VERTEX> vVertexList;
	vector<DWORD> vIndexList;

	int NumCol = _sizeX - 1;
	int NumRow = _sizeZ - 1;

	int vertices = _sizeX * _sizeZ;
	vVertexList.resize(vertices);

	int vertexIndex = 0;
	for (int row = 0; row < _sizeZ; ++row)
	{
		for (int col = 0; col < _sizeX; ++col)
		{
			float x = (col * _cellSize) - (NumCol * _cellSize * 0.5f);
			float y = 0.0f;
			float z = (row * _cellSize) - (NumRow * _cellSize * 0.5f);

			Vec3 pos = Vec3(x, y, z);
			Vec3 normal = Vec3(0, 1, 0);
			Vec4 color = Vec4(1, 1, 1, 1);

			Vec2 uv = Vec2((float)col, (float)(NumRow - row));

			vVertexList[vertexIndex++] = PNCT_VERTEX(pos, normal, color, uv);
		}
	}

	for (int row = 0; row < NumRow; ++row)
	{
		for (int col = 0; col < NumCol; ++col)
		{
			int iTopLeft = row * _sizeX + col;
			int iTopRight = iTopLeft + 1;
			int iBottomLeft = (row + 1) * _sizeX + col;
			int iBottomRight = iBottomLeft + 1;

			vIndexList.push_back(iTopLeft);
			vIndexList.push_back(iBottomLeft);
			vIndexList.push_back(iTopRight);

			vIndexList.push_back(iTopRight);
			vIndexList.push_back(iBottomLeft);
			vIndexList.push_back(iBottomRight);
		}
	}

	m_pMesh = make_shared<UStaticMeshResources>();
	m_pMesh->SetVertexList(vVertexList);
	m_pMesh->SetIndexList(vIndexList);
}


