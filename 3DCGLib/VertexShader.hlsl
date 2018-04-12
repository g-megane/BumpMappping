// �R���X�^���g�o�b�t�@
cbuffer ConstantBuffer : register(b0)
{
    matrix World;          // ���[���h�s��
    matrix View;           // �r���[�s��
    matrix Projection;     // �ˉe�s��
}

struct VS_INPUT
{
    float4 Pos : POSITION;  // ���_�ʒu
    float2 Tex : TEXCOORD0; // UV���W
    float3 Norm : NORMAL;   // �@��
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float3 Norm : NORMAL;
};

// ���_�V�F�[�_�[
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos  = mul(input.Pos, World);
    output.Pos  = mul(output.Pos, View);
    output.Pos  = mul(output.Pos, Projection);
    output.Norm = mul(float4(input.Norm, 0.0f), World);
    output.Tex  = input.Tex;

    return output;
}
