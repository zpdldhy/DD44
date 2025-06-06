#pragma once
#include "Singleton.h"
#include "ViewPortTexture.h"

class AWindActor;


class WindManager : public Singleton<WindManager>
{
public:
    void Init();
    void Tick();
    void Render(); // 이때 ViewPortTexture 사용

    void AddWind(std::shared_ptr<AWindActor> wind);

    ID3D11ShaderResourceView* GetSRV() const;

private:
    std::vector<std::shared_ptr<AWindActor>> m_vWindList;
    std::unique_ptr<ViewPortTexture> m_pWindTexture = nullptr;
};

