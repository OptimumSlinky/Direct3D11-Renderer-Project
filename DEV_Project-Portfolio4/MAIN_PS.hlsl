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

struct PS_Input
{
    float4 positionL : SV_POSITION;
    // float3 color : COLOR;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
};

// Pixel Shader 
float4 PS_Main(PS_Input input) : SV_Target
{
    // Normalize input vector
    input.normal = normalize(input.normal);
    
    float4 outputColor = 0;
    
    float4 textureColor = diffuseTexture.Sample(linearSampler, input.tex);
    
    // Ambient Lighting
    float4 ambientLight = textureColor * 0.5f;
    
    // Directional Lighting
    float dirLightRatio = saturate(dot(-vLightDirection[0].xyz, input.normal));
    outputColor = dirLightRatio * vLightColor[0] * textureColor;
    
    outputColor.a = 1;
    return saturate(outputColor + ambientLight);
}

// Solid Color Pixel Shader
float4 PS_Solid(PS_Input input) : SV_Target
{
    return vOutputColor;
}