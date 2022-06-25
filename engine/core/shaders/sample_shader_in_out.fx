struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D ColorTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 view_proj_mat;
	float4x4 world_mat;
};

cbuffer TerrainConstantBuffer : register(b1)
{
    int2   grid_dim;
    float2 patch_size;
    float3 terrain_origin;
    float  terrain_max_height;
};