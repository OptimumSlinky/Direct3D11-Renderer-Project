// Constant buffer
cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    float4 vLightPosition[2];
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
    
    // Create outputs for different light implementations
    float4 dirOutputColor = 0;
    float4 pointOutputColor = 0;
    float4 spotOutputColor = 0;
    
    // Ambient Lighting
    float4 textureColor = diffuseTexture.Sample(linearSampler, input.tex);
    float4 ambientLight = textureColor * 0.5f;
    
    // Directional Lighting
    float dirLightRatio = saturate(dot(-vLightDirection[0].xyz, input.normal)); // clamp(dot(-LightDir, SurfaceNormal))
    dirOutputColor = dirLightRatio * vLightColor[0] * textureColor; // LightRatio * LightColor * SurfaceColor
    
    // Point Light
    float3 pointLightDir = normalize(vLightPosition[0].xyz - input.positionL); // LightDir = normalize(LightPos - SurfacePos)
    float pointLightRatio = saturate(dot((float3)pointLightDir, input.normal)); // LightRatio = clamp(dot(LightDir, SurfaceNormal)
    pointOutputColor = pointLightRatio * vLightColor[0] * textureColor; // Result = LightRatio * LightColor * SurfaceColor

    // Spotlight
    float3 spotLightDir = normalize(vLightPosition[0].xyz - input.positionL); // LightDir = normalize(LightPos - SurfacePos)
    float spotSurfaceRatio = saturate(dot(-vLightDirection[0].xyz, -input.normal)); // SurfaceRatio = clamp(dot(-LightDir, ConeDir))
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