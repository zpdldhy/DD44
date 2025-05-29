#include "pch.h"
#include "EffectManager.h"
#include "ObjectManager.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "CameraManager.h"


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
        mat->Load(L"../Resources/Texture/Blood2.png", L"../Resources/Shader/blood.hlsl");
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

void EffectManager::MakeBillboardQuadVerts(const Vec3& center, const Vec2& size, const Vec3& cameraPos, std::array<Vec3, 4>& outVerts, std::array<Vec2, 4>& outUVs)
{
    Vec3 toCam = cameraPos - center;
    toCam.y = 0;
    toCam.Normalize();

    Vec3 right = toCam.Cross(Vec3::Up);
    Vec3 up = Vec3::Up;

    Vec3 halfRight = right * (size.x * 0.5f);
    Vec3 halfUp = up * (size.y * 0.5f);

    // Top-left
    outVerts[0] = center - halfRight + halfUp;
    // Top-right
    outVerts[1] = center + halfRight + halfUp;
    // Bottom-left
    outVerts[2] = center - halfRight - halfUp;
    // Bottom-right
    outVerts[3] = center + halfRight - halfUp;

    // UV�� ����
    outUVs[0] = Vec2(0, 0);
    outUVs[1] = Vec2(1, 0);
    outUVs[2] = Vec2(0, 1);
    outUVs[3] = Vec2(1, 1);
}

void EffectManager::PlayEffect(EEffectType type, const Vec3& pos, float maxAngleSpreadDeg, const Vec3& baseVelocity)
{
    auto actor = GetReusableActor(type);
    if (!actor)
        return;

    float speed = baseVelocity.Length();
    if (speed < 0.001f)
        return;

    Vec3 baseDir = baseVelocity;
    baseDir.y = RandomRange(10.f, 30.f);
    baseDir.Normalize();

    // 1. ���� ���� ���� ���� (�¿�, ���Ϸ� 30�� ����)
    float spreadYaw = DegreesToRadians(RandomRange(-30.0f, 30.0f));   // �¿� ����
    float spreadPitch = DegreesToRadians(RandomRange(-20.0f, 20.0f));   // ���Ʒ� ����

    // 2. ȸ�� ����
    Matrix rotYaw = Matrix::CreateRotationY(spreadYaw);
    Matrix rotPitch = Matrix::CreateRotationX(spreadPitch);

    Vec3 dir = Vec3::TransformNormal(baseDir, rotYaw * rotPitch);
    dir.Normalize();

    Vec3 velocity = dir * speed * RandomRange(0.8f, 1.2f);
    Vec3 absDir = Vec3(fabs(dir.x), fabs(dir.y), fabs(dir.z)); // ���⼺�� �����ϵ� scale�� ���̰Բ�

    // 1. StartScale�� ����
    actor->SetStartScale(Vec3(0.3f));

    // 2. EndScale�� ���� ��� + ���� ��ҷ� ������ ȿ��
    Vec3 endScale;
    endScale.x = Lerp(0.8f, 1.3f, RandomRange(0.0f, 1.0f)) + absDir.x * 2.0f;
    endScale.y = Lerp(0.8f, 1.3f, RandomRange(0.0f, 1.0f)) + absDir.y * 0.3f; // y�� �۰�
    endScale.z = Lerp(0.8f, 1.3f, RandomRange(0.0f, 1.0f)) + absDir.z * 2.0f;

    actor->SetEndScale(endScale);
    // Billboard�� ���� ��� ����
    Matrix view = CAMERA->Get3DView();
    Matrix viewInv = view.Invert();
    viewInv._41 = pos.x;
    viewInv._42 = pos.y;
    viewInv._43 = pos.z;
    
    actor->SetWorldMatrix(viewInv);
    
    actor->SetPosition(pos);
    actor->SetGravity(-100.0f);

    //actor->SetStartScale(Vec3(0.3f));
    //actor->SetEndScale(Vec3(RandomRange(0.8f, 1.3f)));

    actor->Play(0.7f, velocity);
}


