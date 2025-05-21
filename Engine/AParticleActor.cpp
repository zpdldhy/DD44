#include "pch.h"
#include "AParticleActor.h"
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "Timer.h"
#include "CameraManager.h"

AParticleActor::AParticleActor()
{
    m_CB_Billboard = make_shared<ConstantBuffer<CB_Billboard>>();
    m_CB_Billboard->Create(6); // b6 ½½·Ô
}

void AParticleActor::Tick()
{
    if (m_CB_Billboard)
    {
        Vec3 scale = GetScale();
        m_CB_Billboard->data.center = GetPosition();
        m_CB_Billboard->data.size = Vec2(scale.x, scale.y); // uniform scale
        m_CB_Billboard->Update();
        m_CB_Billboard->Push();
    }

    AActor::Tick();


    float deltaTime = TIMER->GetDeltaTime();
    m_fTimeAcc += deltaTime;

    float frameTime = 1.0f / m_fFrameRate;
    if (m_fTimeAcc >= frameTime)
    {
        m_fTimeAcc -= frameTime;
        m_iCurrentFrame++;

        int maxFrames = m_iDivisions * m_iDivisions;
        if (m_iCurrentFrame >= maxFrames)
            m_iCurrentFrame = 0;

        int row = m_iCurrentFrame / m_iDivisions;
        int col = m_iCurrentFrame % m_iDivisions;

        float cellSize = 1.0f / m_iDivisions;
        Vec2 uvStart(col * cellSize, row * cellSize);
        Vec2 uvEnd((col + 1) * cellSize, (row + 1) * cellSize);

        SetUV(uvStart, uvEnd);
    }



}

void AParticleActor::SetUV(Vec2 start, Vec2 end)
{
    m_uvStart = start;
    m_uvEnd = end;

    auto mesh = GetMeshComponent();
    auto mat = mesh->GetMaterial();
    mat->SetUVRange(m_uvStart, m_uvEnd);
}

void AParticleActor::InitSpriteAnimation(int divisions, float frameRate)
{
    m_iDivisions = divisions;
    m_fFrameRate = frameRate;

    m_iCurrentFrame = 0;
    m_fTimeAcc = 0.0f;
    m_bPlaying = true;
}
