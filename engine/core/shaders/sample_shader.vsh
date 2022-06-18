#include "sample_shader_in_out.fx"

PSInput main(float3 position : POSITION, float4 uv : TEXCOORD)
{
    PSInput result;

    result.position = mul(view_proj_mat, float4(position, 1.0));
    result.uv = uv;

    return result;
}