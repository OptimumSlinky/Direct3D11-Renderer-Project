// Constant buffer
cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
}

// Input structures
struct VS_Input
{
    float4 positionL : POSITION;
    float4 color : COLOR;
    // float3 normal : NORMAL;
    // float2 texture : TEXCOORD;
};

struct VS_Output
{
    float4 positionH : SV_POSITION;
    float4 color : COLOR;
};

// Vertex shader
VS_Output VS_Main( VS_Input input )
{
    VS_Output output = (VS_Output)0;
    output.positionH = mul(input.positionL, world);
    output.positionH = mul(output.positionH, view);
    output.positionH = mul(output.positionH, projection);
    output.color = input.color;
    return output;
}