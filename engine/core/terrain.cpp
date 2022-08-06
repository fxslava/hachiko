#include "terrain.h"
#include "engine.h"
#include "d3dx12.h"
#include "d3dcompiler.h"
#include "utils.h"


void terrain_grid::init(XMINT2 dimensions_, XMFLOAT3 origin_, float tile_size_, float min_height_, float max_height_)
{
    assert(dimensions_.x > 0);
    assert(dimensions_.y > 0);

    dimensions = dimensions_;
    origin     = origin_;
    tile_size  = tile_size_;
    min_height = min_height_;
    max_height = max_height_;

    const int terrain_tiles_num = dimensions.x * dimensions.y;
    tile_lod.resize(terrain_tiles_num);
    tile_lifetime.resize(terrain_tiles_num);
    transitions_pool.resize(terrain_tiles_num);

    std::fill(tile_lod.begin(), tile_lod.end(), -1);
    std::fill(tile_lifetime.begin(), tile_lifetime.end(), -1);
}


void terrain_grid::push_tile_state_transition(int tile_idx, int old_lod, int new_lod) {
    auto& transition = transitions_pool[tile_idx];
    transition.tile_id = tile_idx;
    transition.old_lod = old_lod;
    transition.new_lod = new_lod;
    transitions.push(&transition);
}


void terrain_grid::update_lods(XMFLOAT3 cam_pos)
{
    XMVECTOR local_terrain_cam_pos = XMLoadFloat3(&cam_pos) - XMLoadFloat3(&origin);

    for (int row = 0; row < dimensions.x; row++) {
        for (int col = 0; col < dimensions.y; col++) {
            const int tile_idx = row + col * dimensions.x;

            const XMVECTOR local_terrain_tile_pos = XMVectorSet(
                (static_cast<float>(row) + 0.5f) * tile_size, 0.0f,
                (static_cast<float>(col) + 0.5f) * tile_size, 0.0f
            );

            const float distance_tile_to_camera = XMVector3Length(local_terrain_tile_pos - local_terrain_cam_pos).m128_f32[0];
            const int new_lod = static_cast<int>(floorf(log2f(std::max(distance_tile_to_camera / TERRAIN_TILE_LOD_TRANSITION_CHARACTERISTIC_DISTANCE, 1.0f))));
            const int old_lod = tile_lod[tile_idx];

            if (new_lod != old_lod) {
                push_tile_state_transition(tile_idx, old_lod, new_lod);
            }

            if (distance_tile_to_camera > TERRAIN_TILE_MAX_DISTANCE) {
                auto& lifetime = tile_lifetime[tile_idx];
                if (++lifetime > TERRAIN_TILE_MAX_LIFETIME) {
                    lifetime = TERRAIN_TILE_MAX_LIFETIME;
                    push_tile_state_transition(tile_idx, old_lod, -1);
                }
            }
        }
    }
}


terrain_grid::TILE_STATE_TRANSITION* terrain_grid::pop_lod_transition() {
    if (!transitions.empty()) {
        auto state_transtition = transitions.front();
        transitions.pop();
        return state_transtition;
    }
    return nullptr;
}


HRESULT terrain_base_c::allocate_resources(const TERRAIN_DESC& desc)
{
    INSTANTIATE_GAME_ENGINE
    INSTANTIATE_GAME_ENGINE_LOWLEVEL

    HRESULT hres = S_OK;
    CK(terrain_render_pass.create_pso(d3d_renderer));

    // Describe and create a shader resource view (SRV) heap for the texture.
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 2;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    CK(d3d_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srv_heap)));

    terrain_origin = XMFLOAT3(desc.origin_x, desc.origin_y, desc.origin_z);
    terrain_dimensions = XMINT2(desc.dimension_x, desc.dimension_y);
    tile_size = desc.tile_size;
    min_height = desc.min_height;
    max_height = desc.max_height;

    constant_buffers_manager->create_constant_buffer(common_terrain_cb_handle, L"COMMON_TERRAIN", sizeof(common_terrain_cb));
    update_constant_buffer();

    grid.init(terrain_dimensions, terrain_origin, tile_size, min_height, max_height);

    return S_OK;
}

void terrain_base_c::update_constant_buffer() {
    common_terrain_cb.origin = XMFLOAT4(terrain_origin.x, terrain_origin.y, terrain_origin.z, 0.f);
    common_terrain_cb.grid_dim = XMINT4(terrain_dimensions.x, terrain_dimensions.y, 0, 0);
    common_terrain_cb.sizes = XMFLOAT4(tile_size, tile_size, min_height, max_height);
}

void terrain_base_c::render(ID3D12GraphicsCommandList* command_list)
{
    auto& engine = engine_c::get_instance();
    terrain_render_pass.setup(command_list);

    std::vector cb_handlers = { engine.common_engine_cb_handle, common_terrain_cb_handle };
    ID3D12DescriptorHeap* cbv_heap;
    constant_buffers_manager->get_cbv_heap(cb_handlers, cbv_heap);

    ID3D12DescriptorHeap* ppHeaps[] = { cbv_heap };
    command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    command_list->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(cbv_heap->GetGPUDescriptorHandleForHeapStart(), 1, d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
    command_list->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(cbv_heap->GetGPUDescriptorHandleForHeapStart(), 0, d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

    if (srv_heap_not_empty) {
        ID3D12DescriptorHeap* ppHeaps[] = { srv_heap.Get() };
        command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
        command_list->SetGraphicsRootDescriptorTable(2, srv_heap->GetGPUDescriptorHandleForHeapStart());
    }

    const int num_instances = common_terrain_cb.grid_dim.x* common_terrain_cb.grid_dim.y;

    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
    command_list->DrawInstanced(4, num_instances, 0, 0);
}


HRESULT terrain_base_c::prepare_frame(ID3D12GraphicsCommandList* command_list)
{
    HRESULT hres;
    CK(constant_buffers_manager->update_constant_buffer(common_terrain_cb_handle, reinterpret_cast<BYTE*>(&common_terrain_cb), sizeof(common_terrain_cb)));

    return S_OK;
}


HRESULT terrain_base_c::update(ID3D12GraphicsCommandList* command_list)
{
    const std::string resource_name = "sample_terrain/image_x0_y1.bmp";

    if (resource_manager->query_resource(resource_name) == resource_manager_c::AVAILABLE)
    {
        const auto& allocation = resource_manager->get_resource(resource_name);

        // Describe and create a SRV for the texture.
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = allocation->desc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        d3d_device->CreateShaderResourceView(allocation->resource->GetResource(), &srvDesc, srv_heap->GetCPUDescriptorHandleForHeapStart());

        srv_heap_not_empty = true;
    }

    return S_OK;
}