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

struct PS_Input
{
    float4 positionL : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD1;
};

// Pixel Shader 
float4 PS_Main(PS_Input input) : SV_Target
{
    float4 outputColor = 0;
    
    // 
    for (int i = 0; i < 2; i++)
    {
        outputColor += saturate(dot((float3) vLightDirection[i], input.normal) * vLightColor[i]);
    }
    
    //outputColor *= diffuseTexture.Sample(linearSampler, input.tex);
    outputColor.a = 1;
    return outputColor;
}

float4 PS_Solid(PS_Input input) : SV_Target
{
    return vOutputColor;
}