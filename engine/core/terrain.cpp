#include "terrain.h"
#include "engine.h"
#include "d3dx12.h"
#include "d3dcompiler.h"
#include "utils.h"

HRESULT terrain_base_c::allocate_resources()
{
    auto& engine = engine_c::get_instance();
    d3d_renderer = engine.get_renderer();
    resource_manager = engine.get_resource_manager();
    constant_buffers_manager = engine.get_constant_buffers_manager();

    const ComPtr<D3D12MA::Allocator> gpu_allocator(d3d_renderer->get_gpu_allocator());

    d3d_device = d3d_renderer->get_d3d_device();

    HRESULT hres = S_OK;
    CK(terrain_render_pass.create_pso(d3d_renderer));

    // Describe and create a shader resource view (SRV) heap for the texture.
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 2;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    CK(d3d_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srv_heap)));

    constant_buffers_manager->create_constant_buffer(common_terrain_cb_handle, L"COMMON_TERRAIN", sizeof(common_terrain_cb));

    return S_OK;
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

HRESULT terrain_base_c::update(ID3D12GraphicsCommandList* command_list)
{
    const std::wstring resource_name = L"sample_terrain/LOD1/image_x0_y1.bmp";

    if (resource_manager->query_resource(resource_name) == resource_manager_c::AVAILABLE)
    {
        const auto allocation = resource_manager->get_resource(resource_name);

        // Describe and create a SRV for the texture.
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = allocation->desc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        d3d_device->CreateShaderResourceView(allocation->resource->GetResource(), &srvDesc, srv_heap->GetCPUDescriptorHandleForHeapStart());

        srv_heap_not_empty = true;
    }

    HRESULT hres;
    CK(constant_buffers_manager->update_constant_buffer(common_terrain_cb_handle, reinterpret_cast<BYTE*>(&common_terrain_cb), sizeof(common_terrain_cb)));

    return S_OK;
}