#include "render_terrain_shader_in_out.fx"

HSInput main(uint vertId : SV_VertexID, uint instId : SV_InstanceID)
{
    HSInput result;

    int2 grid = grid_dim.xy;
    float2 tile_size = sizes.xy;

    float2 patch_uv  = float2( 0.0f, 0.0f);
    switch (vertId) {
    case 1: patch_uv = float2( 1.0f, 0.0f); break;
    case 2: patch_uv = float2( 0.0f, 1.0f); break;
    case 3: patch_uv = float2( 1.0f, 1.0f); break;
    }

    int2   patch_coord = int2(instId % grid.x, min(instId / grid.x, grid.y));
    float2 patch_pos   = (float2(patch_coord) + patch_uv) * tile_size;
    result.position = float3(patch_pos.x, 0.f, patch_pos.y) - terrain_origin.xyz;
    result.uv          = patch_uv;

    return result;
}