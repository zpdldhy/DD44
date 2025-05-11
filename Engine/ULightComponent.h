#pragma once
#include "USceneComponent.h"
#include "LightData.h"

enum class ELightType
{
    Directional,
    Point,
    Spot,
};


class ULightComponent : public USceneComponent
{
private:
    LightData m_tCBLight = {};
    ComPtr<ID3D11Buffer> m_pCBLight;

public:
    void Init() override;
    void PushLightData();
    void Render() override;

public:
    ULightComponent();
    virtual ~ULightComponent() = default;

    // Light 설정
    void SetLightType(ELightType _type) { m_eLightType = _type; }
    void SetColor(const Vec3& _color) { m_vColor = _color; }
    void SetIntensity(float _intensity) { m_fIntensity = _intensity; }
    void SetRange(float _range) { m_fRange = _range; }
    void SetAngle(float _angleDegree) { m_fAngle = _angleDegree; }
    void SetAmbientColor(const Vec3& _color) { m_vAmbientColor = _color; }
    void SetAmbientPower(float _power) { m_fAmbientPower = _power; }
    void SetDirection(const Vec3& _dir)
    {
        m_vDirection = _dir;
        m_vDirection.Normalize();
    }

    // Light 정보 가져오기
    ELightType GetLightType() const { return m_eLightType; }
    const Vec3& GetColor() const { return m_vColor; }
    float GetIntensity() const { return m_fIntensity; }
    float GetRange() const { return m_fRange; }
    float GetAngle() const { return m_fAngle; }
    const Vec3& GetAmbientColor() const { return m_vAmbientColor; }
    float GetAmbientPower() const { return m_fAmbientPower; }

    Vec3 GetDirection() const { return m_vDirection; }

    LightData GetLightData(const Vec3& worldPos) const;
    

protected:
    ELightType m_eLightType = ELightType::Directional;
    Vec3       m_vColor = Vec3(1.0f, 1.0f, 1.0f);
    float      m_fIntensity = 1.0f;
    float      m_fRange = 10.0f;
    float      m_fAngle = 45.0f;
    Vec3 m_vAmbientColor = Vec3(0.1f, 0.1f, 0.1f);
    float m_fAmbientPower = 1.0f;
    Vec3       m_vDirection = Vec3(1.0f, 0.0f, 0.0f);  // 사용자가 지정할 수 있는 방향
};
