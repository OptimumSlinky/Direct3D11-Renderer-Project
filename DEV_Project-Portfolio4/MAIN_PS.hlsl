// Constant buffer

struct LightObj
{
    // Position
    float4 litePosition;
    
    // Color
    float4 liteColor;
    
    // Direction
    float4 liteDirection;
};

cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    float4 vLightPosition[3];
    float4 vLightDirection[3];
    float4 vLightColor[3];
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
    float3 positionW : WORLDPOSITION;

};

// TODO: Create Light Functions

// Pixel Shader 
float4 PS_Main(PS_Input input) : SV_Target
{
    // Normalize input vector
    input.normal = normalize(input.normal);
    
    // Create outputs for different light implementations
    float4 dirOutputColor = 0;
    float4 pointOutputColor = 0;
    float4 spotOutputColor = 0;
    
    // Ambient Lighting
    float4 textureColor = diffuseTexture.Sample(linearSampler, input.tex);
    float4 ambientLight = textureColor * 0.35f;
    
    // Directional Lighting 
    float dirLightRatio = saturate(dot(-vLightDirection[0].xyz, input.normal)); // clamp(dot(-LightDir, SurfaceNormal))
    dirOutputColor = dirLightRatio * vLightColor[2] * textureColor; // LightRatio * LightColor * SurfaceColor
    
    // Point Light (position; no direction)
    float3 pointLightDir = normalize(vLightPosition[0].xyz - input.positionW); // LightDir = normalize(LightPos - SurfacePos)
    float pointLightRatio = saturate(dot((float3)pointLightDir, input.normal)); // LightRatio = clamp(dot(LightDir, SurfaceNormal)
    pointOutputColor = pointLightRatio * vLightColor[0] * textureColor; // Result = LightRatio * LightColor * SurfaceColor

    // Spotlight
    float3 spotLightDir = normalize(vLightPosition[1].xyz - input.positionW); // LightDir = normalize(LightPos - SurfacePos)
    float spotSurfaceRatio = saturate(dot(-spotLightDir, vLightDirection[0].xyz)); // SurfaceRatio = clamp(dot(-LightDir, ConeDir))
    // SpotFactor = (SurfaceRatio > ConeRatio) ? 1:0
    // LightRatio = clamp(dot(LightDir, SurfaceNormal))
    // Output = SpotFactor * LightRatio * LightColor * SurfaceColor
    
    // Set alphas
    dirOutputColor.a = 1;
    pointOutputColor.a = 1;
    
    // Send it
    return saturate(dirOutputColor + pointOutputColor + ambientLight);
}

// Solid Color Pixel Shader
float4 PS_Solid(PS_Input input) : SV_Target
{
    return vOutputColor;
}