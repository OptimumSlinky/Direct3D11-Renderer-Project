#pragma pack_matrix(row_major)

// Constant buffer
cbuffer ConstantBuffer : register(b0)
{
    matrix world[3];
    matrix view;
    matrix projection;
    float4 vLightDirection[2];
    float4 vLightColor[2];
    float4 vOutputColor;
}

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D emissiveTexture : register(t2);
SamplerState linearSampler : register(s0);

// Input structures
struct VS_Input
{
    float3 positionL : POSITION; // Position in local space
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VS_Output
{
    float4 positionH : SV_POSITION; // Position in projection space
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 positionW : WORLDPOSITION;
    // uint instance;
};

// Vertex shader
VS_Output main(VS_Input input)
{
    VS_Output output = (VS_Output)0;
    output.positionH = mul(float4(input.positionL, 1), world[0]); // Stores worldspace
    output.positionW = output.positionH.xyz;
    output.positionH = mul(output.positionH, view); // storing viewspace
    output.positionH = mul(output.positionH, projection);
    output.normal = mul(float4(input.normal, 0), world[0]).xyz;
    output.tex = input.tex, 0;
    return output;
}