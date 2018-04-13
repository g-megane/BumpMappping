Texture2D    Texture : register(t0); // テクスチャー
Texture2D    TextureN : register(t1); //
SamplerState Sampler : register(s0); // サンプラー

struct PS_INPUT
{
    float4 Pos : SV_POSITION; // 座標
    float2 Tex : TEXCOORD0;   // UV座標
};

// ピクセルシェーダー
float4 PS(PS_INPUT input) : SV_TARGET
{
    // バンプマッピング
    float4 textureColor = Texture.Sample(Sampler, input.Tex);
    float3 bumpNormal = 2.0 * TextureN.Sample(Sampler, input.Tex).xyz - 1.0;
    bumpNormal = normalize(bumpNormal);

    return textureColor * (bumpNormal.x + bumpNormal.y + bumpNormal.z);
}
