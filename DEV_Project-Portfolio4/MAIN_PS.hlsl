
cbuffer ConstantBuffer : register(b0)
{
    matrix world[3];
    matrix view;
    matrix projection;
    float4 vLightPosition[3];
    float4 vLightDirection[3];
    float4 vLightColor[3];
    float4 vOutputColor;
    float4 CameraPosition;
}

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D emissiveTexture : register(t2);
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
    float4 specularMaterial = specularTexture.Sample(linearSampler, input.tex);
    float4 emissiveMaterial = emissiveTexture.Sample(linearSampler, input.tex);
    float4 SurfaceSpecColor = float4(1.0f, 1.0f, 1.0f, 1.0f); // For specular calcs; white
            
    // Create outputs for different light implementations
    float4 directionalLight = 0;
    float4 pointLight = 0;
    float4 spotLight = 0;

    // Emissive Lighting
    float4 emissive = emissiveMaterial;
    
    // Ambient Lighting
    float4 ambientLight = textureColor * 0.50f;
    
    // Directional Lighting 
    float dirLightRatio = saturate(dot(normalize(-vLightDirection[2].xyz), input.normal)); // clamp(dot(-LightDir, SurfaceNormal))
    directionalLight = dirLightRatio * vLightColor[2] * textureColor; // LightRatio * LightColor * SurfaceColor

    // Specular for directional light (Clark's version)
    float3 VecToCam = CameraPosition - input.positionW; // float3 vToCam = vCameraPos - vSurfaceWorldPos;
    VecToCam = normalize(VecToCam); // vToCam = normalize(vToCam);
    float3 LightVec = normalize(vLightDirection[2].xyz); // float3 vLightVec = vDirLightDirection; (For point light -> vLightVec = normalize(SurfacePos - vPointLightPos);
    float3 ReflectVec = reflect(LightVec, input.normal); // float3 vReflect = reflect(vLightVec, vSurfaceNormal);
    float SpecDot = saturate(dot(ReflectVec, VecToCam)); // fSpecDot = saturate(dot(vReflect, vToCam));
    SpecDot = pow(SpecDot, 32.0f); // fSpecDot = pow(fSpecDot, fSpecPower);
    float3 SpecDLfinal = SurfaceSpecColor * vLightColor[2] * SpecDot; // float3 vSpecFinal = vSurfaceSpecColor[usually white] * vLightSpecColor[light color] * fSpecDot;

    // Point Light (position; no direction)
    float3 pointLightDir = normalize(vLightPosition[0].xyz - input.positionW); // LightDir = normalize(LightPos - SurfacePos)
    float pointLightRatio = saturate(dot((float3) pointLightDir, input.normal)); // LightRatio = clamp(dot(LightDir, SurfaceNormal)
    
    // Specular for point light (Clark's version)
    float3 ptLightVec = normalize(input.positionW - vLightPosition[0].xyz); // float3 vLightVec = normalize(SurfacePos - vPointLightPos);
    float3 ptReflectVec = reflect(ptLightVec, input.normal); // float3 vReflect = reflect(vLightVec, vSurfaceNormal);
    float ptSpecDot = saturate(dot(ptReflectVec, VecToCam)); // fSpecDot = saturate(dot(vReflect, vToCam));
    ptSpecDot = pow(ptSpecDot, 32.0f); // fSpecDot = pow(fSpecDot, fSpecPower);
    float3 SpecPTfinal = SurfaceSpecColor * vLightColor[2] * ptSpecDot; // float3 vSpecFinal = vSurfaceSpecColor[usually white] * vLightSpecColor[light color] * fSpecDot;

    // Attenuation
    //float attenuation = 1.0f - saturate(length(vLightPosition[0].xyz - input.positionW) / 1.5f);
    pointLight = pointLightRatio * vLightColor[0] * textureColor; // Result = LightRatio * LightColor * SurfaceColor

    // Spotlight
    float3 spotLightDir = normalize(vLightPosition[1].xyz - input.positionW); // LightDir = normalize(LightPos - SurfacePos)
    float spotSurfaceRatio = saturate(dot(-spotLightDir, normalize(vLightDirection[0].xyz))); // SurfaceRatio = clamp(dot(-LightDir, ConeDir))
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
    //return emissive;
    return saturate(ambientLight + emissive + directionalLight + pointLight + spotLight + float4(SpecDLfinal, 0) + float4(SpecPTfinal, 0));
};