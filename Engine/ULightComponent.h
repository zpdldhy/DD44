#pragma once
#include "USceneComponent.h"

enum class ELightType
{
    Directional,
    Point,
    Spot,
};

struct CB_LIGHT
{
    Vec3 vLightColor;
    float fIntensity;

    Vec3 vLightDirection;
    float fRange;

    Vec3 vLightPosition;
    float fAngle;

    int iLightType;
    Vec3 vAmbientColor;

    float fAmbientPower;
    Vec3 padding;
};


class ULightComponent : public USceneComponent
{
private:
    CB_LIGHT m_tCBLight = {};
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

    // Light 정보 가져오기
    ELightType GetLightType() const { return m_eLightType; }
    const Vec3& GetColor() const { return m_vColor; }
    float GetIntensity() const { return m_fIntensity; }
    float GetRange() const { return m_fRange; }
    float GetAngle() const { return m_fAngle; }
    const Vec3& GetAmbientColor() const { return m_vAmbientColor; }
    float GetAmbientPower() const { return m_fAmbientPower; }

protected:
    ELightType m_eLightType = ELightType::Directional;
    Vec3       m_vColor = Vec3(1.0f, 1.0f, 1.0f);
    float      m_fIntensity = 1.0f;
    float      m_fRange = 10.0f;     // Point/Spot용
    float      m_fAngle = 45.0f;     // Spot용
    // Ambient
    Vec3 m_vAmbientColor = Vec3(0.1f, 0.1f, 0.1f); // 기본 색상
    float m_fAmbientPower = 1.0f;                  // 기본 세기
};
