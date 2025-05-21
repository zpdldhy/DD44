#pragma once
#include "AActor.h"
#include "UBoxComponent.h"
#include "EditorData.h"

class AGizmo : public AActor
{
public:
    AGizmo() = default;
    virtual ~AGizmo() = default;

private:
    shared_ptr<UBoxComponent> m_pXBox;
    shared_ptr<UBoxComponent> m_pYBox;
    shared_ptr<UBoxComponent> m_pZBox;

    GizmoAxis m_SelectedAxis = GizmoAxis::NONE;

public:
    void InitGizmo();
    void SetTargetPosition(const Vec3& pos);

    void SetSelectedAxis(GizmoAxis axis) { m_SelectedAxis = axis; }
    GizmoAxis GetSelectedAxis() const { return m_SelectedAxis; }

    shared_ptr<UBoxComponent> GetXBox() const { return m_pXBox; }
    shared_ptr<UBoxComponent> GetYBox() const { return m_pYBox; }
    shared_ptr<UBoxComponent> GetZBox() const { return m_pZBox; }
};

