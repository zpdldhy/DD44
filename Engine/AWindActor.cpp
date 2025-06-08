#include "pch.h"
#include "AWindActor.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "Timer.h"
#include "CameraManager.h"

AWindActor::AWindActor()
{
    auto pMesh = UStaticMeshComponent::CreatePlane();

    // 머티리얼 생성 및 설정
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

    // 위치 이동
    Vec3 pos = GetPosition();
    float time = TIMER->GetDeltaTime();
    pos += m_vVelocity * time;

    Vec3 delta = CAMERA->GetCameraDelta();
    float offsetX = delta.x / (float)g_windowSize.x * 25.f;
    float offsetY = delta.y / (float)g_windowSize.y * 25.f;

    pos.x -= offsetX;  // 보정 강도는 튜닝 가능
    pos.y -= offsetY;

    SetPosition(pos);

    // 화면 바깥 나가면 삭제
    if (pos.x > 1.2f || pos.y < -1.2f)
        m_bDelete = true;
}