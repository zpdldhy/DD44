#include "pch.h"
#include "AWindActor.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"

AWindActor::AWindActor()
{
    auto pMesh = UStaticMeshComponent::CreatePlane();

    // ��Ƽ���� ���� �� ����
    auto pMat = make_shared<UMaterial>();
    pMat->Load(L"../Resources/Texture/Wind.png", L"../Resources/Shader/Wind.hlsl");

    pMesh->SetMaterial(pMat);
    SetMeshComponent(pMesh);

    // ������ ������ ����
    SetScale(Vec3(100.f, 30.f, 1.f));
}

void AWindActor::Init()
{
    AActor::Init();
}
