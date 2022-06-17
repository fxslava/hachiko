#include "terrain.h"
#include "d3dx12.h"
#include "d3dcompiler.h"
#include "utils.h"

HRESULT terrain_base_c::allocate_resources(renderer_i * renderer)
{
    ComPtr<D3D12MA::Allocator> gpu_allocator(renderer->get_gpu_allocator());
    renderer_c* d3d_renderer = reinterpret_cast<renderer_c*>(renderer);

    d3d_device = d3d_renderer->get_d3d_device();

    float aspect_ratio = 1920.0f / 1080.0f;

    HRESULT hres = S_OK;
    CK(sample_shader_pass.create_pso(d3d_renderer));

    Vertex triangleVertices[] =
    {
        { { 0.0f, 0.25f * aspect_ratio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
    };
    const UINT triangle_vertices_size = sizeof(triangleVertices);

    D3D12MA::ALLOCATION_DESC allocationDesc = {};
    allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

    ComPtr<ID3D12Resource> vertex_buffer_resource;
    CK(gpu_allocator->CreateResource(&allocationDesc, &CD3DX12_RESOURCE_DESC::Buffer(triangle_vertices_size), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &vertex_buffer, IID_PPV_ARGS(&vertex_buffer_resource)));

    UINT8* vertex_data;
    const CD3DX12_RANGE read_range(0, 0);
    CK(vertex_buffer_resource->Map(0, &read_range, reinterpret_cast<void**>(&vertex_data)));
    memcpy(vertex_data, triangleVertices, sizeof(triangleVertices));
    vertex_buffer_resource->Unmap(0, nullptr);

    vertex_buffer_view.BufferLocation = vertex_buffer_resource->GetGPUVirtualAddress();
    vertex_buffer_view.StrideInBytes = sizeof(Vertex);
    vertex_buffer_view.SizeInBytes = triangle_vertices_size;

    renderer->wait_for_prev_frame();

    CK(resource_manager.create_resource_factory(d3d_renderer));

    resource_manager.init(fs::current_path() / fs::path("resources"));
    resource_manager.start();

    // Describe and create a shader resource view (SRV) heap for the texture.
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    CK(d3d_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srv_heap)));

    // Describe and create a constant buffer view (CBV) descriptor heap.
    // Flags indicate that this descriptor heap can be bound to the pipeline 
    // and that descriptors contained in it can be referenced by a root table.
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    CK(d3d_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&cbv_heap)));

    D3D12MA::ALLOCATION_DESC constant_buffer_desc = {};
    constant_buffer_desc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

    ComPtr<ID3D12Resource> constant_buffer_resource;
    CK(gpu_allocator->CreateResource(
        &constant_buffer_desc, 
        &CD3DX12_RESOURCE_DESC::Buffer(triangle_vertices_size), 
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &constant_buffer, IID_PPV_ARGS(&constant_buffer_resource)));

    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = constant_buffer_resource->GetGPUVirtualAddress();
    //cbvDesc.SizeInBytes = constantBufferSize;
    d3d_device->CreateConstantBufferView( & cbvDesc, cbv_heap->GetCPUDescriptorHandleForHeapStart());

    return S_OK;
}

void terrain_base_c::render(ID3D12GraphicsCommandList* command_list)
{
    sample_shader_pass.setup(command_list);

    if (srv_heap_not_empty) {
        ID3D12DescriptorHeap* ppHeaps[] = { srv_heap.Get() };
        command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
        command_list->SetGraphicsRootDescriptorTable(0, srv_heap->GetGPUDescriptorHandleForHeapStart());
    }

    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
    command_list->DrawInstanced(3, 1, 0, 0);
}

HRESULT terrain_base_c::update()
{
    const std::wstring resource_name = L"sample_terrain/LOD1/image_x0_y1.bmp";

    if (resource_manager.query_resource(resource_name) == resource_manager_c::AVAILABLE)
    {
        const auto allocation = resource_manager.get_resource(resource_name);

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