#include "pch.h"
#include "AWindActor.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "Timer.h"
#include "CameraManager.h"

AWindActor::AWindActor()
{
    auto pMesh = UStaticMeshComponent::CreatePlane();

    // ��Ƽ���� ���� �� ����
    auto pMat = make_shared<UMaterial>();
    pMat->Load(L"../Resources/Texture/Wind.png", L"../Resources/Shader/Wind.hlsl");

    pMesh->SetMaterial(pMat);
    SetMeshComponent(pMesh);

    m_vVelocity = Vec3(0.4f, -0.4f, 0.0f);
    
}

void AWindActor::Init()
{
    AActor::Init();
}

void AWindActor::Tick()
{
    AActor::Tick();

    // ��ġ �̵�
    Vec3 pos = GetPosition();
    float time = TIMER->GetDeltaTime();
    pos += m_vVelocity * time;


    Vec3 cameraDelta = CAMERA->GetCameraDelta(); 
    pos.x -= cameraDelta.x * 0.015f;
    pos.y -= cameraDelta.y * 0.015f;


    SetPosition(pos);

    // ȭ�� �ٱ� ������ ����
    if (pos.x > 1.2f || pos.y < -1.2f)
        m_bDelete = true;
}