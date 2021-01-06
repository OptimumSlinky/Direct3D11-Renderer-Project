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
    float4 position : POSITION;
    float4 color : COLOR0;
    // float3 normal : NORMAL;
    // float2 texture : TEXCOORD0;
};

// Vertex shader
VS_Input VertexShader( VS_Input input ) : SV_POSITION
{
    VS_Input output = (VS_Input)0;
    output.position = mul(input.position, world);
    output.position = mul(input.position, view);
    output.position = mul(input.position, projection);
    output.color = input.color;
    return output;
}