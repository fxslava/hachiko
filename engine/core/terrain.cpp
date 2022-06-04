#include "terrain.h"
#include "d3dx12.h"
#include "d3dcompiler.h"
#include "utils.h"


HRESULT terrain_base_c::allocate_resources(renderer_i * renderer)
{
    ComPtr<ID3D12Device> d3d_device(renderer->get_d3d_device());
    ComPtr<D3D12MA::Allocator> gpu_allocator(renderer->get_gpu_allocator());
    renderer_c* d3d_renderer = reinterpret_cast<renderer_c*>(renderer);

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
    return S_OK;
}

void terrain_base_c::render(ID3D12GraphicsCommandList* command_list)
{
    sample_shader_pass.setup(command_list);

    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
    command_list->DrawInstanced(3, 1, 0, 0);
}