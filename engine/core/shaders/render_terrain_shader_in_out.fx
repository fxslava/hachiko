Texture2D ColorTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 view_proj_mat;
    float4x4 obj_world_mat;
};

cbuffer TerrainConstantBuffer : register(b1)
{
    int4 grid_dim;
};

struct HSInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct DSInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};
