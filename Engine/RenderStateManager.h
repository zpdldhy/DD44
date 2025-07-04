#pragma once
#include "Singleton.h"
#include "DxState.h"

enum class BlendType
{
    Opaque,
    AlphaBlend,
    Additive,
    Multiply,
    Premultiplied,
    DualSource,
    BloomStrongColor
};

enum class DepthType 
{ 
    ZTestOn_ZWriteOn,
    ZTestOn_ZWriteOff,
    ZTestOff 
};

enum class CullType 
{
    Back,
    Front,
    None
};

enum class SamplerType 
{ 
    Linear,
    Point,
    Clamp,
    Mirror,
    Border
};

enum class RasterizerType
{
    SolidBack,
    SolidNone,
    Wireframe,
    ShadowBias,  
};


struct RenderOption
{
    BlendType blend = BlendType::AlphaBlend;
    DepthType depth = DepthType::ZTestOn_ZWriteOff;
    CullType cull = CullType::None;

    bool bWireframe = false;                
    bool bStencil = false;                  
    SamplerType sampler = SamplerType::Linear; 

    RasterizerType rasterizer = RasterizerType::SolidBack;

    bool operator==(const RenderOption& rhs) const
    {
        return blend == rhs.blend && depth == rhs.depth && cull == rhs.cull &&
            bWireframe == rhs.bWireframe && bStencil == rhs.bStencil && sampler == rhs.sampler
            && rasterizer == rhs.rasterizer;
    }
};

class RenderStateManager : public Singleton<RenderStateManager>
{
public:
    void InitState();
    void Apply(const RenderOption& option);
    void Restore();

private:
    RenderOption m_prevOption;
    RenderOption m_curOption;
    std::stack<RenderOption> m_optionStack;
};