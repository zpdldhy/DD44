#include "pch.h"
#include "UTerrainMeshComponent.h"

void UTerrainMeshComponent::CreateTriangle()
{
	m_vVertexList.resize(3);
	m_vIndexList.resize(3);

	Vec3 vMin = Vec3(0.0f, +0.0f, -1.0f);
	Vec3 vMax = Vec3(+2.0f, +2.0f, +1.0f);

	// Front
	m_vVertexList[0] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[1] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[2] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));

	int iIndex = 0;
	m_vIndexList[iIndex++] = 0; m_vIndexList[iIndex++] = 1; m_vIndexList[iIndex++] = 2;

	CreateVertexBuffer();
	CreateIndexBuffer();
}
void UTerrainMeshComponent::CreatePlane()
{
	m_vVertexList.resize(4);
	m_vIndexList.resize(6);

	Vec3 vMin = Vec3(-100.0f, +0.0f, -100.0f);
	Vec3 vMax = Vec3(+100.0f, +0.0f, +100.0f);

	// Front
	m_vVertexList[0] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[1] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[2] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[3] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 1));

	int iIndex = 0;
	m_vIndexList[iIndex++] = 0; m_vIndexList[iIndex++] = 1; m_vIndexList[iIndex++] = 3;
	m_vIndexList[iIndex++] = 1; m_vIndexList[iIndex++] = 2; m_vIndexList[iIndex++] = 3;

	CreateVertexBuffer();
	CreateIndexBuffer();
}
void UTerrainMeshComponent::CreateCube()
{
	m_vVertexList.resize(24);
	m_vIndexList.resize(36);

	// Transform 넣기 전 Render를 위해 임시로 값 변경
	Vec3 vMin = Vec3(-0.5f, -0.5f, 0.1f);
	Vec3 vMax = Vec3(+0.5f, +0.5f, 0.6f);

	// Front
	m_vVertexList[0] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[1] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[2] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[3] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// Back
	m_vVertexList[4] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	m_vVertexList[5] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[6] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[7] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	// Left
	m_vVertexList[8] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[9] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[10] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[11] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// Right
	m_vVertexList[12] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[13] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[14] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[15] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// UP
	m_vVertexList[16] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[17] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[18] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[19] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// Bottom
	m_vVertexList[20] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[21] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[22] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[23] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 1));

	UINT iIndex = 0;
	auto& I = m_vIndexList;
	// Front
	I[iIndex++] = 0; I[iIndex++] = 1; I[iIndex++] = 2;
	I[iIndex++] = 0; I[iIndex++] = 2; I[iIndex++] = 3;
	// Back
	I[iIndex++] = 6; I[iIndex++] = 5; I[iIndex++] = 4;
	I[iIndex++] = 7; I[iIndex++] = 6; I[iIndex++] = 4;
	// Left
	I[iIndex++] = 8; I[iIndex++] = 9; I[iIndex++] = 10;
	I[iIndex++] = 8; I[iIndex++] = 10; I[iIndex++] = 11;
	// Right
	I[iIndex++] = 12; I[iIndex++] = 13; I[iIndex++] = 14;
	I[iIndex++] = 12; I[iIndex++] = 14; I[iIndex++] = 15;
	// Top
	I[iIndex++] = 16; I[iIndex++] = 17; I[iIndex++] = 18;
	I[iIndex++] = 16; I[iIndex++] = 18; I[iIndex++] = 19;
	// Bottom
	I[iIndex++] = 20; I[iIndex++] = 21; I[iIndex++] = 22;
	I[iIndex++] = 20; I[iIndex++] = 22; I[iIndex++] = 23;

	CreateVertexBuffer();
	CreateIndexBuffer();
}
void UTerrainMeshComponent::CreateSphere(int _sliceCount, int _stackCount)
{
	m_vVertexList.clear();
	m_vIndexList.clear();

	float radius = 0.5f; // 구 반지름

	// Top Vertex
	m_vVertexList.push_back(
		PNCT_VERTEX(
			Vec3(0.0f, +radius, 0.0f),
			Vec3(0.0f, +1.0f, 0.0f),
			Vec4(1, 0, 0, 1),
			Vec2(0.0f, 0.0f)
		)
	);

	float phiStep = DD_PI / _stackCount;
	float thetaStep = 2.0f * DD_PI / _sliceCount;

	// Middle vertices
	for (int i = 1; i <= _stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		for (int j = 0; j <= _sliceCount; ++j)
		{
			float theta = j * thetaStep;

			Vec3 pos(
				radius * sinf(phi) * cosf(theta),
				radius * cosf(phi),
				radius * sinf(phi) * sinf(theta)
			);

			Vec3 normal = pos;
			normal.Normalize();
			Vec2 texCoord(theta / (2 * DD_PI), phi / DD_PI);

			m_vVertexList.push_back(
				PNCT_VERTEX(pos, normal, Vec4(1, 0, 0, 1), texCoord)
			);
		}
	}

	// Bottom Vertex
	m_vVertexList.push_back(
		PNCT_VERTEX(
			Vec3(0.0f, -radius, 0.0f),
			Vec3(0.0f, -1.0f, 0.0f),
			Vec4(1, 0, 0, 1),
			Vec2(0.0f, 1.0f)
		)
	);

	// Top stack
	for (int i = 1; i <= _sliceCount; ++i)
	{
		m_vIndexList.push_back(0);
		m_vIndexList.push_back(i + 1);
		m_vIndexList.push_back(i);
	}

	// Middle stacks
	int baseIndex = 1;
	int ringVertexCount = _sliceCount + 1;
	for (int i = 0; i < _stackCount - 2; ++i)
	{
		for (int j = 0; j < _sliceCount; ++j)
		{
			m_vIndexList.push_back(baseIndex + i * ringVertexCount + j);
			m_vIndexList.push_back(baseIndex + i * ringVertexCount + j + 1);
			m_vIndexList.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			m_vIndexList.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			m_vIndexList.push_back(baseIndex + i * ringVertexCount + j + 1);
			m_vIndexList.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	// Bottom stack
	int southPoleIndex = (int)m_vVertexList.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;

	for (int i = 0; i < _sliceCount; ++i)
	{
		m_vIndexList.push_back(southPoleIndex);
		m_vIndexList.push_back(baseIndex + i);
		m_vIndexList.push_back(baseIndex + i + 1);
	}

	CreateVertexBuffer();
	CreateIndexBuffer();
}
void UTerrainMeshComponent::CreateGrid(int _col, int _row, float _cellSize)
{
	int vertices = (_col + 1) * (_row + 1);
	int indices = _col * _row * 6;

	m_vVertexList.resize(vertices);
	m_vIndexList.resize(indices);

	int vertexIndex = 0;
	for (int row = 0; row <= _row; ++row)
	{
		for (int col = 0; col <= _col; ++col)
		{
			float x = (col * _cellSize) - (_col * _cellSize * 0.5f);
			float y = 0.0f;
			float z = (row * _cellSize) - (_row * _cellSize * 0.5f);

			Vec3 pos = Vec3(x, y, z);
			Vec3 normal = Vec3(0, 1, 0);
			Vec4 color = Vec4(1, 1, 1, 1);
			Vec2 uv = Vec2((float)col / _col, (float)row / _row); // 한장의 텍스처
			//Vec2 uv = Vec2((float)col, (float)row); // 바둑판 배열

			m_vVertexList[vertexIndex++] = PNCT_VERTEX(pos, normal, color, uv);
		}
	}

	int iIndex = 0;
	for (int row = 0; row < _row; ++row)
	{
		for (int col = 0; col < _col; ++col)
		{
			int iTopLeft = row * (_col + 1) + col;
			int iTopRight = iTopLeft + 1;
			int iBottomLeft = (row + 1) * (_col + 1) + col;
			int iBottomRight = iBottomLeft + 1;

			m_vIndexList[iIndex++] = iTopLeft;
			m_vIndexList[iIndex++] = iBottomLeft;
			m_vIndexList[iIndex++] = iTopRight;

			m_vIndexList[iIndex++] = iTopRight;
			m_vIndexList[iIndex++] = iBottomLeft;
			m_vIndexList[iIndex++] = iBottomRight;
		}
	}

	CreateVertexBuffer();
	CreateIndexBuffer();
}

