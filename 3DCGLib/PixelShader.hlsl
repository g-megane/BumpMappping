Texture2D    Texture : register(t0); // テクスチャー
SamplerState Sampler : register(s0); // サンプラー

struct PS_INPUT
{
    float4 Pos : SV_POSITION; // 座標
    float2 Tex : TEXCOORD0;   // UV座標
};

// ピクセルシェーダー
float4 PS(PS_INPUT input) : SV_TARGET
{
    // 
	return Texture.Sample(Sampler, input.Tex);
}