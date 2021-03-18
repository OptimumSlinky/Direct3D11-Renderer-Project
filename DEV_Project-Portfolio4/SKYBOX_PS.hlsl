struct VS_Output
{
    float4 positionP : SV_POSITION; // Position in projection space
    float3 tex : TEXCOORD0;
};

textureCUBE skyboxTexture : register(t0);
SamplerState linearSampler : register(s0);

float4 main(VS_Output vertexOut) : SV_TARGET
{
    return skyboxTexture.Sample(linearSampler, vertexOut.tex);
	
}