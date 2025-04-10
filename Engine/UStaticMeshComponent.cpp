#include "pch.h"
#include "UStaticMeshComponent.h"

void UStaticMeshComponent::Init()
{
}

void UStaticMeshComponent::Tick()
{
}

void UStaticMeshComponent::PreRender()
{
}

void UStaticMeshComponent::Render()
{
}

void UStaticMeshComponent::PostRender()
{
}

void UStaticMeshComponent::Destroy()
{
}

void UStaticMeshComponent::CreateCube()
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
void UStaticMeshComponent::CreateTriangle()
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
void UStaticMeshComponent::CreatePlane()
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