#include "Header.hlsli"


VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 local = float4(input.p.xyz, 1.0f);

    // 흔들리는 방향 벡터 (X축 기준으로 휘기)
    float wave = sin(input.p.y * 20.0f + g_fGameTime * 10.0f) * 0.02f;
    local.x += wave; // X 방향으로 꼬부랑

    output.p = mul(local, g_matWorld);
    output.t = input.t;
    return output;
}

//VS_OUT VS(VS_IN input)
//{
//    VS_OUT output;
//    float4 local = float4(input.p.xyz, 1.0f); 
//    output.p = mul(local, g_matWorld); 

//    output.t = input.t; // UV 그대로 사용

//    return output;
//}

float4 PS(VS_OUT input) : SV_Target
{
    float4 result = g_txDiffuseA.Sample(sample, input.t);

// 0.0 ~ 1.0 사이: UV.x 기준
    float t = input.t.x;

// 둥글게 나타나기 위해 앞에서 빠르게 증가
    float head = smoothstep(0.0f, 0.25f, t);

// 뒤는 taper로 부드럽게 감소 (지금의 핵심)
    float tail = smoothstep(1.0f, 0.6f, t); // 뒤쪽이 부드럽게 빠짐

// 최종 알파: 앞에서 빠르게 생성되고, 뒤는 점점 줄어듦
    float alpha = head * tail;

    result.a *= saturate(alpha * 0.7f); // 전체 투명도 조절
    return result;
}
