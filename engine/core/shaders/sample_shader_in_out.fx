struct PSInput
{
    float4 position : SV_POSITION;
    float4 uv : TEXCOORD;
};

Texture2D ColorTexture : register(t0);
SamplerState Sampler : register(s0);
