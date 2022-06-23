struct PSInput
{
    float4 position : SV_POSITION;
    float4 uv : TEXCOORD;
};

Texture2D ColorTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 view_proj_mat;
};

cbuffer WorldConstantBuffer : register(b1)
{
    float4x4 world_mat;
};
