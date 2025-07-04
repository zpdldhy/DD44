#pragma once
#include "ATerrainTileActor.h"

struct Bounds
{
    Vec3 vMin = Vec3();
    Vec3 vMax = Vec3();

    bool Contains(const Vec3& pos) const
    {
        return (pos.x >= vMin.x && pos.x <= vMax.x &&
            pos.z >= vMin.z && pos.z <= vMax.z);
    }
};

struct QuadTreeNode
{
    int iCellIndex = -1;
    Box bounds = Box();
    std::vector<UINT> vActorIndices;
    std::unique_ptr<QuadTreeNode> pChildren[4];
};

class QuadTree
{
public:
    void Create(int numCols, int numRows, float cellSize);
    void UpdateActor(std::shared_ptr<AActor> actor);

    int  CalculateMaxDepth(int numCols, int numRows);
    void Subdivide(QuadTreeNode* node, int depth, int maxDepth);
    
    void InsertActor(QuadTreeNode* node, std::shared_ptr<AActor> actor);
    void RemoveActorFromAllNodes(QuadTreeNode* node, UINT actorIndex);

    std::vector<UINT> FindNearbyActorIndices(shared_ptr<AActor> _actor);
    
    QuadTreeNode* GetRoot() const { return m_pRoot.get(); }
	std::vector<QuadTreeNode*> GetLeafs() const { return m_pLeafs; }

private:
    std::unique_ptr<QuadTreeNode> m_pRoot = nullptr;
    std::vector<QuadTreeNode*> m_pLeafs;
};

