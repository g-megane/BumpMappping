Texture2D    Texture : register(t0); // �e�N�X�`���[
SamplerState Sampler : register(s0); // �T���v���[

struct PS_INPUT
{
    float4 Pos : SV_POSITION; // ���W
    float2 Tex : TEXCOORD0;   // UV���W
};

// �s�N�Z���V�F�[�_�[
float4 PS(PS_INPUT input) : SV_TARGET
{
    // 
	return Texture.Sample(Sampler, input.Tex);
}