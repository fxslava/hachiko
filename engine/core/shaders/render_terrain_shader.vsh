#include "render_terrain_shader_in_out.fx"

HSInput main(uint vertId : SV_VertexID)
{
    HSInput result;

    int patchVertId = vertId & 3;
    int primId = vertId >> 2;

    float2 uv = float2(float(patchVertId & 1), float(patchVertId >> 1));

    int2   patch_coord = int2(primId % grid_dim.x, min(primId / grid_dim.x, grid_dim.y));
    float2 patch_pos   = (float2(patch_coord) + uv) * patch_size;
    result.position = terrain_origin + float3(patch_pos, 0);
    result.uv = uv;

    return result;
}