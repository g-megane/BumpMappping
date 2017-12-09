// �R���X�^���g�o�b�t�@
cbuffer ConstantBuffer : register(b0)
{
    matrix World;          // ���[���h�s��
    matrix View;           // �r���[�s��
    matrix Projection;     // �ˉe�s��
    float4 vLightDir[2];   // ���C�g�̍��W
    float4 vLightColor[2]; // ���C�g�̐F
    float4 vOutputColor;   // ���C�g�ȊO�̃��f���̐F
}

struct VS_INPUT
{
    float4 Pos : POSITION; // ���_�ʒu
    float3 Norm : NORMAL;  // �@���x�N�g��
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
};

// ���_�V�F�[�_�[
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos  = mul(input.Pos, World);
    output.Pos  = mul(output.Pos, View);
    output.Pos  = mul(output.Pos, Projection);
    output.Norm = mul(float4(input.Norm, 0.0f), World).xyz;

    return output;
}
