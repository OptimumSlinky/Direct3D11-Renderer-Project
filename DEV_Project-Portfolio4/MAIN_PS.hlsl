// Constant buffer

struct LightObj
{
    float4 position;
    float4 color;
    float4 direction;
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
float4 DrawAmbientLight(float4 textureColor, float intensity)
{
    float4 finalOutput = textureColor * intensity;
    return finalOutput;
};

float4 DrawDirectionLight(LightObj light, PS_Input pixelInput, float4 textureColor)
{
    float directionRatio = saturate(dot(-light.direction.xyz, pixelInput.normal));
    float4 finalOutput = directionRatio * light.color * textureColor;
    finalOutput.a = 1;
    return finalOutput;
};

float4 DrawPointLight(LightObj light, PS_Input pixelInput, float4 textureColor)
{
    float3 pointDirection = normalize(light.position.xyz - pixelInput.positionW); // LightDir = normalize(LightPos - SurfacePos)
    float pointRatio = saturate(dot((float3) pointDirection, pixelInput.normal)); // LightRatio = clamp(dot(LightDir, SurfaceNormal)
    float4 finalOutput = pointRatio * light.color * textureColor; // Result = LightRatio * LightColor * SurfaceColor
    finalOutput.a = 1;
    return finalOutput;
};



// Pixel Shader 
float4 PS_Main(PS_Input input) : SV_Target
{
    // Normalize input vector
    input.normal = normalize(input.normal);
    
    // Get Texture Color
    float4 textureColor = diffuseTexture.Sample(linearSampler, input.tex);
    
    // Set Ambient Light
    //float4 ambientLight = DrawAmbientLight(textureColor, 0.35f);
    
    // Set Directional Light
    //float4 directionalLight = DrawDirectionLight(vLightDirection[0], input, textureColor);
    
    // Set Point Light
    //float4 pointLight = DrawPointLight(vLightPosition[0], input, textureColor);
    
    // Create outputs for different light implementations
    float4 directionalLight = 0;
    float4 pointLight = 0;
    float4 spotOutputColor = 0;
    
    // Ambient Lighting
    float4 ambientLight = textureColor * 0.35f;
    
    // Directional Lighting 
    float dirLightRatio = saturate(dot(-vLightDirection[0].xyz, input.normal)); // clamp(dot(-LightDir, SurfaceNormal))
    directionalLight = dirLightRatio * vLightColor[2] * textureColor; // LightRatio * LightColor * SurfaceColor
    
    // Point Light (position; no direction)
    float3 pointLightDir = normalize(vLightPosition[0].xyz - input.positionW); // LightDir = normalize(LightPos - SurfacePos)
    float pointLightRatio = saturate(dot((float3) pointLightDir, input.normal)); // LightRatio = clamp(dot(LightDir, SurfaceNormal)
    pointLight = pointLightRatio * vLightColor[0] * textureColor; // Result = LightRatio * LightColor * SurfaceColor

    // Spotlight
    float3 spotLightDir = normalize(vLightPosition[1].xyz - input.positionW); // LightDir = normalize(LightPos - SurfacePos)
    float spotSurfaceRatio = saturate(dot(-spotLightDir, vLightDirection[0].xyz)); // SurfaceRatio = clamp(dot(-LightDir, ConeDir))
    // SpotFactor = (SurfaceRatio > ConeRatio) ? 1:0
    // LightRatio = clamp(dot(LightDir, SurfaceNormal))
    // Output = SpotFactor * LightRatio * LightColor * SurfaceColor
    
    // Set alphas
    directionalLight.a = 1;
    pointLight.a = 1;
    
    // Send it
    return saturate(ambientLight + directionalLight + pointLight);
}