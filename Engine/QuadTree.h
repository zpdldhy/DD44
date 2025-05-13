// QuadTreeNode.h
#pragma once

struct QuadTreeNode
{
    int iCellIndex = -1;
    std::vector<UINT> vActorIndices;

    bool bRender = false;
    bool bCollision = false;

    std::unique_ptr<QuadTreeNode> pChildren[4];
};

class QuadTree
{
public:
    void Create(int rootCellIndex, int maxDepth);
    void Subdivide(QuadTreeNode* node, int depth, int maxDepth);
    int  GenerateUniqueCellIndex();

private:
    std::unique_ptr<QuadTreeNode> m_pRoot;

};

