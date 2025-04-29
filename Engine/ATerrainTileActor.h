#pragma once
#include "AActor.h"
#include "UTerrainMeshComponent.h"
#include "Texture.h"

class ATerrainTileActor : public AActor
{
public:
	UTerrainMeshComponent* m_pTerrainMeshComponent;
	shared_ptr<Texture> m_pHeightMap;

	int m_iNumCols = 10;
	int m_iNumRows = 10;
	float m_fCellSize = 10.0f;

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Render() override;
};

// 그리드 크기, 셀 사이즈 관리
// 높이맵 데이터