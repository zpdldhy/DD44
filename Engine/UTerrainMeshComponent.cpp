#include "pch.h"
#include "UTerrainMeshComponent.h"
#include "UMaterial.h"

void UTerrainMeshComponent::Init()
{
    USceneComponent::Init();
}

void UTerrainMeshComponent::Tick()
{
    USceneComponent::Tick();
}

void UTerrainMeshComponent::Render()
{
    USceneComponent::Render();
	PreRender();
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

void UTerrainMeshComponent::CreateGrid(int numCellsX, int numCellsZ, float cellSize)
{
    if (m_pMesh)
    {
        return;
    }

    int vertexCountX = numCellsX + 1;
    int vertexCountZ = numCellsZ + 1;

    vector<PNCT_VERTEX> vVertexList;
    vector<DWORD> vIndexList;

    int vertices = vertexCountX * vertexCountZ;
    vVertexList.resize(vertices);

    int vertexIndex = 0;
    for (int row = 0; row < vertexCountZ; ++row)
    {
        for (int col = 0; col < vertexCountX; ++col)
        {
            float x = (col * cellSize) - ((vertexCountX - 1) * cellSize * 0.5f);
            float y = 0.0f;
            float z = (row * cellSize) - ((vertexCountZ - 1) * cellSize * 0.5f);

            Vec3 pos = Vec3(x, y, z);
            Vec3 normal = Vec3(0, 1, 0);
            Vec4 color = Vec4(1, 1, 1, 1);

            Vec2 uv = Vec2((float)col, (float)(numCellsZ - row));

            vVertexList[vertexIndex++] = PNCT_VERTEX(pos, normal, color, uv);
        }
    }

    for (int row = 0; row < numCellsZ; ++row)
    {
        for (int col = 0; col < numCellsX; ++col)
        {
            int iTopLeft = row * vertexCountX + col;
            int iTopRight = iTopLeft + 1;
            int iBottomLeft = (row + 1) * vertexCountX + col;
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
