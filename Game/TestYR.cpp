#include "pch.h"
#include "TestYR.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"

void TestYR::Init()
{
	m_pStaticMesh = make_shared<UStaticMeshComponent>();
	m_pStaticMesh->CreateCube();
	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Glow.hlsl");
	m_pStaticMesh->SetMaterial(material);

}
void TestYR::Update()
{
    
}
void TestYR::Render()
{
	m_pStaticMesh->Render();
}
