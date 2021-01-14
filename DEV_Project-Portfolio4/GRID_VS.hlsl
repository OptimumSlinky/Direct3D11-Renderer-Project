#pragma pack_matrix(row_major)

cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

// Vertex shader
PS_INPUT main( VS_INPUT input ) 
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(float4(input.Pos, 1), World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Color = input.Color;
    return output;
}