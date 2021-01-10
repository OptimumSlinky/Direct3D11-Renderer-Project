// Constant buffer
cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    float4 vOutputColor;
}

Texture2D diffuseTexture : register(t0);
SamplerState linearSampler : register(s0);

// Input structures
struct PS_Input
{
    float4 positionL : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texture : TEXCOORD1;
};

// Pixel Shader 
float4 PS_Main(PS_Input input) : SV_Target
{
    float4 outputColor = 0;
    outputColor *= diffuseTexture.Sample(linearSampler, input.texture);
    outputColor.a = 1;
    return outputColor;
}