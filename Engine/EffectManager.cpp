#include "pch.h"
#include "EffectManager.h"
#include "ObjectManager.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"


void EffectManager::Init()
{
    for (int i = 0; i < 50; ++i) 
    {
        for (int t = 0; t <= (int)EEffectType::Dust; ++t)
        {
            auto actor = CreateEffectActor((EEffectType)t);
            actor->m_szName = L"Effect";
            m_mEffectPool[(EEffectType)t].push_back(actor);
            OBJECT->AddActor(actor); 
        }
    }
}

shared_ptr<AEffectActor> EffectManager::CreateEffectActor(EEffectType type)
{
    auto actor = make_shared<AEffectActor>(type);

    // �޽�
    auto mesh = UStaticMeshComponent::CreatePlane();
    actor->SetMeshComponent(mesh);

    // ��Ƽ����
    auto mat = make_shared<UMaterial>();
    switch (type)
    {
    case EEffectType::Blood:
        mat->Load(L"../Resources/Texture/Blood2.png", L"../Resources/Shader/SpriteUV.hlsl");
        break;
    case EEffectType::Dust:
        mat->Load(L"../Resources/Texture/Dust.png", L"../Resources/Shader/SpriteUV.hlsl");
        break;
    }

    mesh->SetMaterial(mat);

    // ���� ����
    actor->SetLoop(false);
    actor->SetAutoDestroy(false);
    actor->SetRender(false);
    actor->SetDuration(0.5f);

    return actor;
}

shared_ptr<AEffectActor> EffectManager::GetReusableActor(EEffectType type)
{
    for (auto& actor : m_mEffectPool[type])
    {
        if (!actor->IsActive())
            return actor;
    }

    return nullptr;
}

void EffectManager::PlayEffect(EEffectType type, const Vec3& pos, float maxAngleSpreadDeg, const Vec3& baseVelocity)
{
    auto actor = GetReusableActor(type);
    if (!actor)
        return;

    // ȸ�� ���� ��� (baseVelocity �������� ���� ���� ��ź)
    float angleRad = DegreesToRadians(RandomRange(-maxAngleSpreadDeg * 0.5f, maxAngleSpreadDeg * 0.5f));
    float speed = baseVelocity.Length();
    float zSpread = RandomRange(0.3f, 0.8f); // ���� Ƣ�� ����

    Vec3 dir = Vec3(cos(angleRad), sin(angleRad), zSpread);
    dir.Normalize();
    Vec3 velocity = dir * speed;

    // ���⿡ �´� ȸ�� ���� (XY ��� ���� �ð���)
    float visualAngle = atan2(dir.y, dir.x) * (180.0f / 3.14159265f);

    actor->SetPosition(pos);
    actor->SetRotation(Vec3(0, 0, visualAngle));
    actor->SetGravity(-30.0f); // �߷� ����
    actor->SetStartScale(Vec3(0.3f)); // �۰� ����
    actor->SetEndScale(Vec3(RandomRange(0.8f, 1.3f))); // �� �ϰ� ����

    actor->Play(.7f, velocity); // ������ �����
}

