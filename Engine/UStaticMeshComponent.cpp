#include "pch.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "AActor.h"

void UStaticMeshComponent::Init()
{
	USceneComponent::Init();

	for (auto& child : m_vChild)
	{
		child->SetOwner(GetOwner());
		child->Init();
	}
}

void UStaticMeshComponent::Tick()
{
	if (m_pAnim)
	{
		auto mat = m_pAnim->GetBoneAnim(m_targetBoneIndex);
		m_matAnim = m_matBone * mat;
	}
	USceneComponent::Tick();

	for (auto& child : m_vChild)
	{
		child->Tick();
	}
}

void UStaticMeshComponent::PreRender()
{
	USceneComponent::Render();
	// Mesh Bind
	if (m_pMesh) { m_pMesh->Bind(); }

	// Material Bind
	if (m_pMaterial) { m_pMaterial->Bind(); }
}

void UStaticMeshComponent::PostRender()
{
	if (m_pMesh->GetIndexCount() <= 0)
		DC->Draw(m_pMesh->GetVertexCount(), 0);
	else
		DC->DrawIndexed(m_pMesh->GetIndexCount(), 0, 0);
}

void UStaticMeshComponent::Destroy()
{
}

shared_ptr<UStaticMeshComponent> UStaticMeshComponent::CreateRay(Vec3 _vStart, Vec3 _vEnd)
{
	static shared_ptr<UStaticMeshResources> pMesh = nullptr;
	auto pMeshComponent = make_shared<UStaticMeshComponent>();
	pMeshComponent->SetMeshPath(L"Ray");

	// Mesh Data Setting
	vector<PNCT_VERTEX> vVertexList;

	vVertexList.resize(2);

	Vec3 vMin = Vec3(+0.0f, +0.0f, -0.5f);
	Vec3 vMax = Vec3(+0.0f, +0.0f, +0.5f);

	vVertexList[0] = PNCT_VERTEX(Vec3(_vStart.x, _vStart.y, _vStart.z), Vec3(0, 0, 0), Vec4(1, 1, 0, 1), Vec2(0, 0));
	vVertexList[1] = PNCT_VERTEX(Vec3(_vEnd.x, _vEnd.y, _vEnd.z), Vec3(0, 0, 0), Vec4(1, 1, 0, 1), Vec2(0, 1));

	// Mesh Setting
	pMesh = make_shared<UStaticMeshResources>();
	pMesh->SetVertexList(vVertexList);
	pMesh->SetTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	pMesh->Create();

	// StaticMeshComponent Setting
	pMeshComponent->SetMesh(pMesh);
	return pMeshComponent;
}

shared_ptr<UStaticMeshComponent> UStaticMeshComponent::CreateTriangle()
{
	static shared_ptr<UStaticMeshResources> pMesh = nullptr;

	auto pMeshComponent = make_shared<UStaticMeshComponent>();
	pMeshComponent->SetMeshPath(L"Triangle");

	if (pMesh)
	{
		pMeshComponent->SetMesh(pMesh);
		return pMeshComponent;
	}

	// Mesh Data Setting
	vector<PNCT_VERTEX> vVertexList;
	vector<DWORD> vIndexList;

	vVertexList.resize(3);
	vIndexList.resize(3);

	Vec3 vMin = Vec3(-0.5f, -0.5f, -0.0f);
	Vec3 vMax = Vec3(+0.5f, +0.5f, +0.0f);

	// Front
	vVertexList[0] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	vVertexList[1] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	vVertexList[2] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));

	int iIndex = 0;
	vIndexList[iIndex++] = 0; vIndexList[iIndex++] = 1; vIndexList[iIndex++] = 2;

	// Mesh Setting
	pMesh = make_shared<UStaticMeshResources>();
	pMesh->SetVertexList(vVertexList);
	pMesh->SetIndexList(vIndexList);
	pMesh->Create();

	// StaticMeshComponent Setting
	pMeshComponent->SetMesh(pMesh);
	return pMeshComponent;
}

shared_ptr<UStaticMeshComponent> UStaticMeshComponent::CreatePlane()
{
	static shared_ptr<UStaticMeshResources> pMesh = nullptr;

	auto pMeshComponent = make_shared<UStaticMeshComponent>();
	pMeshComponent->SetMeshPath(L"Plane");

	if (pMesh)
	{
		pMeshComponent->SetMesh(pMesh);
		return pMeshComponent;
	}

	// Mesh Data Setting
	vector<PNCT_VERTEX> vVertexList;
	vector<DWORD> vIndexList;

	vVertexList.resize(4);
	vIndexList.resize(6);

	Vec3 vMin = Vec3(-0.5f, -0.5f, -0.f);
	Vec3 vMax = Vec3(+0.5f, +0.5f, +0.f);

	// Front
	vVertexList[0] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	vVertexList[1] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	vVertexList[2] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	vVertexList[3] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 1));

	int iIndex = 0;
	vIndexList[iIndex++] = 0; vIndexList[iIndex++] = 1; vIndexList[iIndex++] = 3;
	vIndexList[iIndex++] = 1; vIndexList[iIndex++] = 2; vIndexList[iIndex++] = 3;

	// Mesh Setting
	pMesh = make_shared<UStaticMeshResources>();
	pMesh->SetVertexList(vVertexList);
	pMesh->SetIndexList(vIndexList);
	pMesh->Create();

	pMeshComponent->SetMesh(pMesh);
	return pMeshComponent;
}

shared_ptr<UStaticMeshComponent> UStaticMeshComponent::CreateCube()
{
	static shared_ptr<UStaticMeshResources> pMesh = nullptr;

	auto pMeshComponent = make_shared<UStaticMeshComponent>();
	pMeshComponent->SetMeshPath(L"Cube");

	if (pMesh)
	{
		pMeshComponent->SetMesh(pMesh);
		return pMeshComponent;
	}

	// Mesh Data Setting
	vector<PNCT_VERTEX> vVertexList;
	vector<DWORD> vIndexList;

	vVertexList.resize(24);
	vIndexList.resize(36);

	// Transform 넣기 전 Render를 위해 임시로 값 변경
	Vec3 vMin = Vec3(-0.5f, -0.5f, -0.5f);
	Vec3 vMax = Vec3(+0.5f, +0.5f, +0.5f);

	// Front
	vVertexList[0] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	vVertexList[1] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	vVertexList[2] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	vVertexList[3] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// Back
	vVertexList[4] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	vVertexList[5] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	vVertexList[6] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	vVertexList[7] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	// Left
	vVertexList[8] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(-1, 0, 0), Vec4(1, 0, 0, 1), Vec2(0, 1));
	vVertexList[9] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(-1, 0, 0), Vec4(1, 0, 0, 1), Vec2(0, 0));
	vVertexList[10] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(-1, 0, 0), Vec4(1, 0, 0, 1), Vec2(1, 0));
	vVertexList[11] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(-1, 0, 0), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// Right
	vVertexList[12] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(1, 0, 0), Vec4(1, 0, 0, 1), Vec2(0, 1));
	vVertexList[13] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(1, 0, 0), Vec4(1, 0, 0, 1), Vec2(0, 0));
	vVertexList[14] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(1, 0, 0), Vec4(1, 0, 0, 1), Vec2(1, 0));
	vVertexList[15] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(1, 0, 0), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// UP
	vVertexList[16] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0, 1, 0), Vec4(1, 0, 0, 1), Vec2(0, 1));
	vVertexList[17] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0, 1, 0), Vec4(1, 0, 0, 1), Vec2(0, 0));
	vVertexList[18] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0, 1, 0), Vec4(1, 0, 0, 1), Vec2(1, 0));
	vVertexList[19] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(0, 1, 0), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// Bottom
	vVertexList[20] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0, -1, 0), Vec4(1, 0, 0, 1), Vec2(0, 1));
	vVertexList[21] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, -1, 0), Vec4(1, 0, 0, 1), Vec2(0, 0));
	vVertexList[22] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, -1, 0), Vec4(1, 0, 0, 1), Vec2(1, 0));
	vVertexList[23] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0, -1, 0), Vec4(1, 0, 0, 1), Vec2(1, 1));

	UINT iIndex = 0;
	auto& I = vIndexList;
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

	// Mesh Setting
	pMesh = make_shared<UStaticMeshResources>();
	pMesh->SetVertexList(vVertexList);
	pMesh->SetIndexList(vIndexList);
	pMesh->Create();

	pMeshComponent->SetMesh(pMesh);
	return pMeshComponent;
}

shared_ptr<UStaticMeshComponent> UStaticMeshComponent::CreateSphere(int _sliceCount, int _stackCount)
{
	static shared_ptr<UStaticMeshResources> pMesh = nullptr;

	auto pMeshComponent = make_shared<UStaticMeshComponent>();
	pMeshComponent->SetMeshPath(L"Sphere");

	if (pMesh)
	{
		pMeshComponent->SetMesh(pMesh);
		return pMeshComponent;
	}

	// Mesh Data Setting
	vector<PNCT_VERTEX> vVertexList;
	vector<DWORD> vIndexList;

	vVertexList.clear();
	vIndexList.clear();

	float radius = 0.5f; // 구 반지름

	// Top Vertex
	vVertexList.push_back(
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

			vVertexList.push_back(
				PNCT_VERTEX(pos, normal, Vec4(1, 0, 0, 1), texCoord)
			);
		}
	}

	// Bottom Vertex
	vVertexList.push_back(
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
		vIndexList.push_back(0);
		vIndexList.push_back(i + 1);
		vIndexList.push_back(i);
	}

	// Middle stacks
	int baseIndex = 1;
	int ringVertexCount = _sliceCount + 1;
	for (int i = 0; i < _stackCount - 2; ++i)
	{
		for (int j = 0; j < _sliceCount; ++j)
		{
			vIndexList.push_back(baseIndex + i * ringVertexCount + j);
			vIndexList.push_back(baseIndex + i * ringVertexCount + j + 1);
			vIndexList.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			vIndexList.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			vIndexList.push_back(baseIndex + i * ringVertexCount + j + 1);
			vIndexList.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	// Bottom stack
	int southPoleIndex = (int)vVertexList.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;

	for (int i = 0; i < _sliceCount; ++i)
	{
		vIndexList.push_back(southPoleIndex);
		vIndexList.push_back(baseIndex + i);
		vIndexList.push_back(baseIndex + i + 1);
	}

	// Mesh Setting
	pMesh = make_shared<UStaticMeshResources>();
	pMesh->SetVertexList(vVertexList);
	pMesh->SetIndexList(vIndexList);
	pMesh->Create();

	pMeshComponent->SetMesh(pMesh);
	return pMeshComponent;
}