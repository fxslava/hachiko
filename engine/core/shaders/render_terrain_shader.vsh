#include "render_terrain_shader_in_out.fx"

HSInput main(uint vertId : SV_VertexID, uint instId : SV_InstanceID)
{
    HSInput result;

    float2 patch_uv    = float2(float(vertId & 1), float((vertId >> 1) & 1));
    int2   patch_coord = int2(instId % 32, min(instId / 32, grid_dim.y));
    float2 patch_pos   = float2(patch_coord) + patch_uv;
    result.position    = float3(patch_pos, 0);
    result.uv          = patch_uv;

    return result;
}