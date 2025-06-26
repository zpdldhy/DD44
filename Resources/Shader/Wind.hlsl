#include "Header.hlsli"


VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 local = float4(input.p.xyz, 1.0f);

    // ��鸮�� ���� ���� (X�� �������� �ֱ�)
    float wave = sin(input.p.y * 20.0f + g_fGameTime * 10.0f) * 0.02f;
    local.x += wave; // X �������� ���ζ�

    output.p = mul(local, g_matWorld);
    output.t = input.t;
    return output;
}

//VS_OUT VS(VS_IN input)
//{
//    VS_OUT output;
//    float4 local = float4(input.p.xyz, 1.0f); 
//    output.p = mul(local, g_matWorld); 

//    output.t = input.t; // UV �״�� ���

//    return output;
//}

float4 PS(VS_OUT input) : SV_Target
{
    float4 result = g_txDiffuseA.Sample(sample, input.t);

// 0.0 ~ 1.0 ����: UV.x ����
    float t = input.t.x;

// �ձ۰� ��Ÿ���� ���� �տ��� ������ ����
    float head = smoothstep(0.0f, 0.25f, t);

// �ڴ� taper�� �ε巴�� ���� (������ �ٽ�)
    float tail = smoothstep(1.0f, 0.6f, t); // ������ �ε巴�� ����

// ���� ����: �տ��� ������ �����ǰ�, �ڴ� ���� �پ��
    float alpha = head * tail;

    result.a *= saturate(alpha * 0.7f); // ��ü ���� ����
    return result;
}
