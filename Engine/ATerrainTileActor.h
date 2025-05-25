#pragma once
#include "AActor.h"
#include "UTerrainMeshComponent.h"
#include "Texture.h"

class ATerrainTileActor : public AActor
{
public:
    shared_ptr<class UTerrainMeshComponent> m_pTerrainMeshComponent = nullptr;
    shared_ptr<Texture> m_pHeightMap;

    int m_iNumCols = 20;
    int m_iNumRows = 20;
    float m_fCellSize = 10.0f;

    std::vector<std::shared_ptr<class APawn>> m_vObjects;

public:
    virtual void Init() override;
    virtual void Tick() override;
    virtual void Render() override;

public:
    bool CreateTerrain(const wstring& _texturePath, const wstring& _shaderPath);
    bool CreateTerrainFromHeightMap(const std::wstring& heightMapPath, const std::wstring& texturePath, const std::wstring& shaderPath);

    float GetHeightAt(float x, float z);
    int GetCellIndexAt(float x, float z);

    int GetNumCols() const { return m_iNumCols; }
    int GetNumRows() const { return m_iNumRows; }
    float GetCellSize() const { return m_fCellSize; }

    void SetNumCols(int cols) { m_iNumCols = cols; }
    void SetNumRows(int rows) { m_iNumRows = rows; }
    void SetCellSize(float size) { m_fCellSize = size; }
};
