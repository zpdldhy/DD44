#include "pch.h"
#include "AGizmo.h"

void AGizmo::InitGizmo()
{
    // X축 박스
    m_pXBox = make_shared<UBoxComponent>();
    m_pXBox->SetLocalScale(Vec3(0.1f, 0.1f, 1.0f));
    m_pXBox->SetLocalPosition(Vec3(1.0f, 0.0f, 0.0f)); // 길이의 끝쪽
    m_pXBox->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
    //AddComponent(m_pXBox);

    // Y축 박스
    m_pYBox = make_shared<UBoxComponent>();
    m_pYBox->SetLocalScale(Vec3(0.1f, 1.0f, 0.1f));
    m_pYBox->SetLocalPosition(Vec3(0.0f, 1.0f, 0.0f));
    m_pYBox->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
    //AddComponent(m_pYBox);

    // Z축 박스
    m_pZBox = make_shared<UBoxComponent>();
    m_pZBox->SetLocalScale(Vec3(1.0f, 0.1f, 0.1f));
    m_pZBox->SetLocalPosition(Vec3(0.0f, 0.0f, 1.0f));
    m_pZBox->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
    //AddComponent(m_pZBox);
}

void AGizmo::SetTargetPosition(const Vec3& pos)
{
    SetPosition(pos);

    // 로컬 → 월드 갱신 필요 시 Tick() 또는 수동 갱신으로
    if (m_pXBox) m_pXBox->Tick();
    if (m_pYBox) m_pYBox->Tick();
    if (m_pZBox) m_pZBox->Tick();
}
