#include "pch.h"
#include "UTerrainMeshComponent.h"
#include "UMaterial.h"

void UTerrainMeshComponent::Init()
{
}

void UTerrainMeshComponent::Tick()
{
}

void UTerrainMeshComponent::PreRender()
{
	// IA Setting
	UINT Strides = sizeof(PNCT_VERTEX);
	UINT Offsets = 0;
	DC->IASetVertexBuffers(0, 1, m_pMesh->GetVertexBuffer().GetAddressOf(), &Strides, &Offsets);
	DC->IASetIndexBuffer(m_pMesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Material Render
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
		return;

	// Mesh Data Setting
	vector<PNCT_VERTEX> vVertexList;
	vector<DWORD> vIndexList;

	int NumCol = _sizeX - 1;
	int NumRow = _sizeZ - 1;

	int vertices = _sizeX * _sizeZ;
	int indices = NumCol * NumRow * 6;

	vVertexList.resize(vertices);
	vIndexList.resize(indices);

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
			// *텍스처 uv 0~1 범위 설정 시 픽셀단위로 쪼개져서 단색으로 보이는 것으로 추정
			//Vec2 uv = Vec2((float)col / NumCol, (float)row / NumRow);
			Vec2 uv = Vec2(col * 1.0f, (NumRow - row) * 1.0f);
			//Vec2 uv = Vec2(col * 0.1f, row * 0.1f); // 임의로 타일링

			vVertexList[vertexIndex++] = PNCT_VERTEX(pos, normal, color, uv);
		}
	}

	int iIndex = 0;
	for (int row = 0; row < NumRow; ++row)
	{
		for (int col = 0; col < NumCol; ++col)
		{
			int iTopLeft = row * _sizeX + col;
			int iTopRight = iTopLeft + 1;
			int iBottomLeft = (row + 1) * _sizeX + col;
			int iBottomRight = iBottomLeft + 1;

			vIndexList[iIndex++] = iTopLeft;
			vIndexList[iIndex++] = iBottomLeft;
			vIndexList[iIndex++] = iTopRight;

			vIndexList[iIndex++] = iTopRight;
			vIndexList[iIndex++] = iBottomLeft;
			vIndexList[iIndex++] = iBottomRight;
		}
	}

	// Mesh Setting
	m_pMesh = make_shared<UStaticMeshResources>();
	m_pMesh->SetVertexList(vVertexList);
	m_pMesh->SetIndexList(vIndexList);
}

