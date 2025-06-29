#include "pch.h"
#include "QuadTree.h"

void QuadTree::Create(int numCols, int numRows, float cellSize)
{
    float halfWidth = numCols * 0.5f * cellSize;
    float halfHeight = numRows * 0.5f * cellSize;

    Box rootBounds;
    rootBounds.vMin = Vec3(-halfWidth, -100.f, -halfHeight);
    rootBounds.vMax = Vec3(halfWidth, 100.f, halfHeight);

    m_pRoot = std::make_unique<QuadTreeNode>();
    m_pRoot->iCellIndex = 0;
    m_pRoot->bounds = rootBounds;

    Subdivide(m_pRoot.get(), 0, CalculateMaxDepth(numCols, numRows));
}

void QuadTree::UpdateActor(std::shared_ptr<AActor> actor)
{
    if (!actor || !actor->m_bUpdateQuadTree)
        return;

    RemoveActorFromAllNodes(m_pRoot.get(), actor->m_Index);
    InsertActor(m_pRoot.get(), actor);
}

void QuadTree::Subdivide(QuadTreeNode* node, int depth, int maxDepth)
{
    if (depth >= maxDepth)
        return;

    const Vec3& min = node->bounds.vMin;
    const Vec3& max = node->bounds.vMax;
    Vec3 center = (min + max) * 0.5f;

    // 4 분할
    node->pChildren[0] = std::make_unique<QuadTreeNode>();
    node->pChildren[0]->iCellIndex = node->iCellIndex * 4 + 1;
    node->pChildren[0]->bounds.vMin = Vec3(min.x, -100.f, min.z);
    node->pChildren[0]->bounds.vMax = Vec3(center.x, 100.f, center.z);

    node->pChildren[1] = std::make_unique<QuadTreeNode>();
    node->pChildren[1]->iCellIndex = node->iCellIndex * 4 + 2;
    node->pChildren[1]->bounds.vMin = Vec3(center.x, -100.f, min.z);
    node->pChildren[1]->bounds.vMax = Vec3(max.x, 100.f, center.z);

    node->pChildren[2] = std::make_unique<QuadTreeNode>();
    node->pChildren[2]->iCellIndex = node->iCellIndex * 4 + 3;
    node->pChildren[2]->bounds.vMin = Vec3(min.x, -100.f, center.z);
    node->pChildren[2]->bounds.vMax = Vec3(center.x, 100.f, max.z);

    node->pChildren[3] = std::make_unique<QuadTreeNode>();
    node->pChildren[3]->iCellIndex = node->iCellIndex * 4 + 4;
    node->pChildren[3]->bounds.vMin = Vec3(center.x, -100.f, center.z);
    node->pChildren[3]->bounds.vMax = Vec3(max.x, 100.f, max.z);

    // 재귀
    for (int i = 0; i < 4; ++i)
    {
        Subdivide(node->pChildren[i].get(), depth + 1, maxDepth);

        if (depth + 1 == maxDepth)
			m_pLeafs.push_back(node->pChildren[i].get()); // 리프 노드 저장
    }
}

int QuadTree::CalculateMaxDepth(int numCols, int numRows)
{
    int larger = (numCols > numRows) ? numCols : numRows;
    int depth = 0;
    while ((1 << depth) < larger)
    {
        ++depth;
    }
    return depth;
}

void QuadTree::InsertActor(QuadTreeNode* node, std::shared_ptr<AActor> actor)
{
    const Vec3& pos = actor->GetPosition();

    if (!node->bounds.Contains(pos))
        return;

    // 리프면 바로 등록
    if (node->pChildren[0] == nullptr)
    {
        node->vActorIndices.push_back(actor->m_Index);
        return;
    }

    // 자식 모두 재귀
    for (int i = 0; i < 4; ++i)
    {
        InsertActor(node->pChildren[i].get(), actor);
    }

    // 리프가 아니면 현재 노드에도 넣기 (모든 경로 포함)
    node->vActorIndices.push_back(actor->m_Index);
}


void QuadTree::RemoveActorFromAllNodes(QuadTreeNode* node, UINT actorIndex)
{
    if (!node)
        return;

    auto& indices = node->vActorIndices;
    indices.erase(std::remove(indices.begin(), indices.end(), actorIndex), indices.end());

    for (int i = 0; i < 4; ++i)
    {
        RemoveActorFromAllNodes(node->pChildren[i].get(), actorIndex);
    }
}

std::vector<UINT> QuadTree::FindNearbyActorIndices(shared_ptr<AActor> _actor)
{
    std::vector<UINT> result;

    // 1. 액터가 포함된 리프 노드 찾기
    QuadTreeNode* pCenterNode = nullptr;
    Vec3 actorPos = _actor->GetPosition();

    for (QuadTreeNode* pLeaf : m_pLeafs)
    {
        if (pLeaf->bounds.Contains(actorPos))
        {
            pCenterNode = pLeaf;
            break;
        }
    }

    if (pCenterNode == nullptr)
        return result; // 없으면

    // 2. 주변 3x3 노드 탐색
    for (QuadTreeNode* pLeaf : m_pLeafs)
    {
        const Vec3& min = pLeaf->bounds.vMin;
        const Vec3& max = pLeaf->bounds.vMax;

        // 중심 노드 경계 확장 (한 칸 여유)
        const Vec3& cMin = pCenterNode->bounds.vMin;
        const Vec3& cMax = pCenterNode->bounds.vMax;

        if (max.x >= cMin.x - (cMax.x - cMin.x) &&
            min.x <= cMax.x + (cMax.x - cMin.x) &&
            max.z >= cMin.z - (cMax.z - cMin.z) &&
            min.z <= cMax.z + (cMax.z - cMin.z))
        {
            result.insert(result.end(), pLeaf->vActorIndices.begin(), pLeaf->vActorIndices.end());
        }
    }

    return result;
}
