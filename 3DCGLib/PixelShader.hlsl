Texture2D    Texture : register(t0); // �e�N�X�`���[
Texture2D    TextureN : register(t1); //
SamplerState Sampler : register(s0); // �T���v���[

struct PS_INPUT
{
    float4 Pos : SV_POSITION; // ���W
    float2 Tex : TEXCOORD0;   // UV���W
};

// �s�N�Z���V�F�[�_�[
float4 PS(PS_INPUT input) : SV_TARGET
{
    // �o���v�}�b�s���O
    float4 textureColor = Texture.Sample(Sampler, input.Tex);
    float3 bumpNormal = 2.0 * TextureN.Sample(Sampler, input.Tex).xyz - 1.0;
    bumpNormal = normalize(bumpNormal);

    return textureColor * (bumpNormal.x + bumpNormal.y + bumpNormal.z);
}
