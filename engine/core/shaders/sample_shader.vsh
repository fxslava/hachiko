#include "sample_shader_in_out.fx"

PSInput main(float4 position : POSITION, float4 uv : TEXCOORD)
{
    PSInput result;

    result.position = mul(position, view_proj_mat);
    result.uv = uv;

    return result;
}