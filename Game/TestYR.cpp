#include "pch.h"
#include "TestYR.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "Input.h"
#include "Timer.h"

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
    static bool bGlow = false;
    static float glowTimer = 0.0f;

    // R 키를 누르면 발광 시작
    if (INPUT->GetButtonDown(R))
    {
        bGlow = true;
        glowTimer = 0.1f; // 1초 동안 발광
    }

    // 발광 중이면 타이머 감소
    if (bGlow)
    {
        glowTimer -= TIMER->GetDeltaTime();
        if (glowTimer <= 0.0f)
        {
            bGlow = false;
            glowTimer = 0.0f;
        }
    }

    // 현재 발광 값 계산
    float glowPower = bGlow ? 2.0f : 0.0f;

    // Material에 업데이트
    m_pStaticMesh->GetMaterial()->SetGlowParams(
        glowPower,
        Vec3(1.0f, 0.0f, 0.0f) // 빨간색
    );
}
void TestYR::Render()
{
	m_pStaticMesh->Render();
}
