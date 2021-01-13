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

//// Input structures
//struct VS_Input
//{
//    float3 positionL : POSITION;
//    // float3 color : COLOR;
//    float3 normal : NORMAL;
//    float2 tex : TEXCOORD0;
//};

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
    input.normal = normalize(input.normal);
    float4 outputColor = 0;
    float4 textureColor = diffuseTexture.Sample(linearSampler, input.tex);
    float4 ambientLight = textureColor * 0.5f;
    float dirLightRatio;
    dirLightRatio = saturate(dot(-vLightDirection[0].xyz, input.normal));
    //// 
    //for (int i = 0; i < 2; i++)
    //{
    //    outputColor += saturate(dot((float3) vLightDirection[i], input.normal) * vLightColor[i]);
    //}
    
    outputColor = dirLightRatio * vLightColor[0] *textureColor;
    outputColor.a = 1;
    return saturate(outputColor + ambientLight);
}

float4 PS_Solid(PS_Input input) : SV_Target
{
    return vOutputColor;
}