// Constant buffer
cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
}

Texture2D diffuseTexture : register(t0);
SamplerState linearSampler : register(s0);

// Input structures
struct VS_Input
{
    float4 positionL : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texture : TEXTURECOORD0;
};

struct VS_Output
{
    float4 positionH : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float texture : TEXTURECOORD0;
};

// Vertex shader
VS_Output VS_Main( VS_Input input )
{
    VS_Output output = (VS_Output)0;
    output.positionH = mul(input.positionL, world);
    output.positionH = mul(output.positionH, view);
    output.positionH = mul(output.positionH, projection);
    output.normal = mul(float4(input.normal, 1), world).xyz;
    output.texture = input.texture;
    return output;
}