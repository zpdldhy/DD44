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

    // R Ű�� ������ �߱� ����
    if (INPUT->GetButtonDown(R))
    {
        bGlow = true;
        glowTimer = 0.1f; // 1�� ���� �߱�
    }

    // �߱� ���̸� Ÿ�̸� ����
    if (bGlow)
    {
        glowTimer -= TIMER->GetDeltaTime();
        if (glowTimer <= 0.0f)
        {
            bGlow = false;
            glowTimer = 0.0f;
        }
    }

    // ���� �߱� �� ���
    float glowPower = bGlow ? 2.0f : 0.0f;

    // Material�� ������Ʈ
    m_pStaticMesh->GetMaterial()->SetGlowParams(
        glowPower,
        Vec3(1.0f, 0.0f, 0.0f) // ������
    );
}
void TestYR::Render()
{
	m_pStaticMesh->Render();
}
