// Constant buffer
cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    float4 vLightDirection[2];
    float4 vLightColor[2];
    float4 vOutputColor;
}

Texture2D diffuseTexture : register(t0);
SamplerState linearSampler : register(s0);

// Input structures
struct VS_Input
{
    float4 positionL : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
};

struct VS_Output
{
    float4 positionH : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float tex : TEXCOORD0;
};

// Vertex shader
VS_Output VS_Main( VS_Input input )
{
    VS_Output output = (VS_Output)0;
    output.positionH = mul(input.positionL, world);
    output.positionH = mul(output.positionH, view);
    output.positionH = mul(output.positionH, projection);
    output.normal = mul(float4(input.normal, 1), world).xyz;
    // output.tex = input.tex;
    return output;
}