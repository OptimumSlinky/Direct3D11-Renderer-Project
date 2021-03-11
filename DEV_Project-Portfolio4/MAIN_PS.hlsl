// Constant buffer

cbuffer ConstantBuffer : register(b0)
{
    matrix world[3];
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
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 positionW : WORLDPOSITION;

};

// Pixel Shader 
float4 PS_Main(PS_Input input) : SV_Target
{
    // Normalize input vector
    input.normal = normalize(input.normal);
    
    // Get Texture Color
    float4 textureColor = diffuseTexture.Sample(linearSampler, input.tex);
            
    // Create outputs for different light implementations
    float4 directionalLight = 0;
    float4 pointLight = 0;
    float4 spotLight = 0;
    
    // Ambient Lighting
    float4 ambientLight = textureColor * 0.30f;
    
    // Directional Lighting 
    float dirLightRatio = saturate(dot(-vLightDirection[2].xyz, input.normal)); // clamp(dot(-LightDir, SurfaceNormal))
    directionalLight = dirLightRatio * vLightColor[2] * textureColor; // LightRatio * LightColor * SurfaceColor
    // Specular addition
    //float dlViewDir = normalize(view - input.positionW); // normalize(camera world position - surface position)
    //float dlHalfVec = normalize((-vLightDirection[2].xyz) + dlViewDir); // normalize((-light direction) + view direction)
    //float dlIntensity; // max(clamp(dot(normal, half vector)) specular power, 0)
    //float dlReflect; // light color * specular intensity * intensity

    
    // Point Light (position; no direction)
    float3 pointLightDir = normalize(vLightPosition[0].xyz - input.positionW); // LightDir = normalize(LightPos - SurfacePos)
    float pointLightRatio = saturate(dot((float3) pointLightDir, input.normal)); // LightRatio = clamp(dot(LightDir, SurfaceNormal)
    // Attenuation
    //float attenuation = 1.0f - saturate(length(vLightPosition[0].xyz - input.positionW) / 1.5f);
    pointLight = pointLightRatio /** attenuation*/ * vLightColor[0] * textureColor; // Result = LightRatio * LightColor * SurfaceColor

    // Spotlight
    float3 spotLightDir = normalize(vLightPosition[1].xyz - input.positionW); // LightDir = normalize(LightPos - SurfacePos)
    float spotSurfaceRatio = saturate(dot(-spotLightDir, vLightDirection[0].xyz)); // SurfaceRatio = clamp(dot(-LightDir, ConeDir))
    float spotFactor = spotSurfaceRatio > 0.8 ? 1 : 0; //SpotFactor = (SurfaceRatio > ConeRatio) ? 1:0
    float spotLightRatio = saturate(dot((float3)spotLightDir, input.normal)); // LightRatio = clamp(dot(LightDir, SurfaceNormal))
    // Attenuation for cone edge
    float spotAtten = 1.0 - saturate((0.7 - spotSurfaceRatio) / (0.7 - 0.9)); // 1 - clamp (inner cone ration - surface ratio) / (inner cone ratio - outer cone ration)
    spotLight = spotFactor * spotLightRatio * vLightColor[1] * textureColor * spotAtten; // Output = SpotFactor * LightRatio * LightColor * SurfaceColor
     
    // Set alphas
    directionalLight.a = 1;
    pointLight.a = 1;
    spotLight.a = 1;

    // Send it
    return saturate(ambientLight + directionalLight + pointLight + spotLight);
}