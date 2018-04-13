// コンスタントバッファ
cbuffer ConstantBuffer : register(b0)
{
    matrix World;          // ワールド行列
    matrix View;           // ビュー行列
    matrix Projection;     // 射影行列
}

struct VS_INPUT
{
    float4 Pos : POSITION;  // 頂点位置
    float2 Tex : TEXCOORD0; // UV座標
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

// 頂点シェーダー
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos  = mul(input.Pos, World);
    output.Pos  = mul(output.Pos, View);
    output.Pos  = mul(output.Pos, Projection);
    output.Tex  = input.Tex;

    return output;
}
