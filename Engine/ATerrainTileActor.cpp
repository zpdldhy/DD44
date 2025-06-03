#include "pch.h"
#include "ATerrainTileActor.h"
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "APawn.h"
#include "UBoxComponent.h"

void ATerrainTileActor::Init()
{
	AActor::Init();
}

void ATerrainTileActor::Tick()
{
	AActor::Tick();
}

void ATerrainTileActor::Render()
{
	AActor::Render();
}

bool ATerrainTileActor::CreateTerrain(const wstring& _texturePath, const wstring& _shaderPath)
{
	m_pTerrainMeshComponent = make_shared<UTerrainMeshComponent>();
	if (!m_pTerrainMeshComponent)
	{
		return false;
	}

	m_pTerrainMeshComponent->CreateGrid(m_iNumCols, m_iNumRows, m_fCellSize);

	auto material = make_shared<UMaterial>();
	material->Load(_texturePath, _shaderPath);	
	m_pTerrainMeshComponent->SetMaterial(material);

	shared_ptr<UBoxComponent> box = make_shared<UBoxComponent>();
	float width = m_iNumCols * m_fCellSize;
	float depth = m_iNumRows * m_fCellSize;
	box->SetLocalScale(Vec3(width, 1.0f, depth));
	box->SetLocalPosition(Vec3(0, -0.5f, 0));
	box->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	SetShapeComponent(box);

	SetMeshComponent(m_pTerrainMeshComponent);
	m_pTerrainMeshComponent->MeshCreate();

	return true;
}

bool ATerrainTileActor::CreateTerrainFromHeightMap(const std::wstring& heightMapPath, const std::wstring& texturePath, const std::wstring& shaderPath)
{
	using namespace DirectX;

	ScratchImage heightImage;
	if (FAILED(LoadFromWICFile(heightMapPath.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, heightImage)))
	{
		return false;
	}

	const Image* img = heightImage.GetImage(0, 0, 0);
	if (!img)
	{
		return false;
	}

	UINT8* pixels = img->pixels;
	int width = static_cast<int>(img->width);
	int height = static_cast<int>(img->height);

	m_iNumCols = width;
	m_iNumRows = height;

	m_pTerrainMeshComponent = make_shared<UTerrainMeshComponent>();
	m_pTerrainMeshComponent->CreateGrid(width, height, m_fCellSize);

	auto vertexList = m_pTerrainMeshComponent->GetMesh()->GetVertexList();
	auto newVertexList = vertexList;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int idx = y * width + x;
			UINT8 gray = pixels[y * img->rowPitch + x];
			float heightValue = (gray / 255.0f * 500.0f) - 300.0f;

			newVertexList[idx].pos.y = heightValue;
		}
	}

	m_pTerrainMeshComponent->GetMesh()->SetVertexList(newVertexList);
	m_pTerrainMeshComponent->MeshCreate();

	auto material = make_shared<UMaterial>();
	material->Load(texturePath, shaderPath);

	m_pTerrainMeshComponent->SetMaterial(material);
	SetMeshComponent(m_pTerrainMeshComponent);

	return true;
}

float ATerrainTileActor::GetHeightAt(float x, float z)
{
	auto vertexList = m_pTerrainMeshComponent->GetMesh()->GetVertexList();
	int numCols = m_iNumCols;
	int numRows = m_iNumRows;
	float cellSize = m_fCellSize;

	// 타일 월드 위치를 고려하여 로컬 좌표 변환
	Vec3 tilePos = GetPosition();
	float localX = x - tilePos.x;
	float localZ = z - tilePos.z;

	float halfWidth = (numCols) * 0.5f * cellSize;
	float halfHeight = (numRows) * 0.5f * cellSize;

	// 좌상단을 기준으로 변환
	float gridX = (localX + halfWidth) / cellSize;
	float gridZ = (localZ + halfHeight) / cellSize;

	int col = (int)gridX;
	int row = (int)gridZ;

	if (col < 0 || col >= numCols || row < 0 || row >= numRows)
	{
		return 0.0f;
	}

	int vertexCountX = numCols + 1;

	int idxLT = row * vertexCountX + col;
	int idxRT = idxLT + 1;
	int idxLB = (row + 1) * vertexCountX + col;
	int idxRB = idxLB + 1;

	float yLT = vertexList[idxLT].pos.y;
	float yRT = vertexList[idxRT].pos.y;
	float yLB = vertexList[idxLB].pos.y;
	float yRB = vertexList[idxRB].pos.y;

	float dx = gridX - col;
	float dz = gridZ - row;

	float height = 0.0f;
	if (dx + dz < 1.0f)
	{
		height = yLT + (yRT - yLT) * dx + (yLB - yLT) * dz;
	}
	else
	{
		height = yRB + (yLB - yRB) * (1.0f - dx) + (yRT - yRB) * (1.0f - dz);
	}

	return height + tilePos.y;
}

int ATerrainTileActor::GetCellIndexAt(float x, float z)
{
	int numCols = m_iNumCols;
	int numRows = m_iNumRows;
	float cellSize = m_fCellSize;

	float halfWidth = numCols * 0.5f * cellSize;
	float halfHeight = numRows * 0.5f * cellSize;

	float gridX = (x + halfWidth) / cellSize;
	float gridZ = (z + halfHeight) / cellSize;

	int col = (int)gridX;
	int row = (int)gridZ;

	if (col < 0 || col >= numCols || row < 0 || row >= numRows)
	{
		return -1; // 범위 밖
	}

	return row * numCols + col;
}
