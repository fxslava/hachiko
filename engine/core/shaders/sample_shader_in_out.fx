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
    float4x4 obj_world_mat;
    float4 padding[8];
};
