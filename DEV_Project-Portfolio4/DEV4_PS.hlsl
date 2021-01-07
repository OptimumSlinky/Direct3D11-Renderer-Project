// Constant buffer
cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
}

// Input structures
struct PS_Input
{
    float4 positionL : POSITION;
    float4 color : COLOR;
    // float3 normal : NORMAL;
    // float2 texture : TEXCOORD;
};

// Pixel Shader 
float4 PS_Main(PS_Input input) : SV_Target
{
    float4 outputColor = input.color;
    return outputColor;
}