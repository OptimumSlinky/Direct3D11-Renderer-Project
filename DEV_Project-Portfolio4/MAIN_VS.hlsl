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
    float4 CameraPosition;
}

Texture2D diffuseTexture : register(t0);
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
    float4 positionP : SV_POSITION; // Position in projection space
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 positionW : WORLDPOSITION;
    // uint instance;
};

// Vertex shader
VS_Output main( VS_Input input, uint instance : SV_InstanceID )
{
    VS_Output output = (VS_Output)0;
    output.positionP = mul(float4(input.positionL, 1), world[instance]); // Stores worldspace
    output.positionW = output.positionP.xyz;
    output.positionP = mul(output.positionP, view); // storing viewspace
    output.positionP = mul(output.positionP, projection);
    output.normal = mul(float4(input.normal, 0), world[instance]).xyz;
    output.tex = input.tex;
  
    return output;
}