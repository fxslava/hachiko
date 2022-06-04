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

    // Create the vertex buffer.
    {
        // Define the geometry for a triangle.
        Vertex triangleVertices[] =
        {
            { { 0.0f, 0.25f * aspect_ratio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        };

        const UINT vertexBufferSize = sizeof(triangleVertices);

        D3D12MA::ALLOCATION_DESC allocationDesc = {};
        allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

        ComPtr<ID3D12Resource> vertexBuffer;
        CK(gpu_allocator->CreateResource(&allocationDesc, &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &m_vertexBuffer, IID_PPV_ARGS(&vertexBuffer)));

        // Copy the triangle data to the vertex buffer.
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        CK(vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
        vertexBuffer->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        m_vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = vertexBufferSize;
    }

    /*// Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        CK(d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue = 1;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            CK(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        renderer->wait_for_prev_frame();
    }*/

    renderer->wait_for_prev_frame();
    return S_OK;
}

void terrain_base_c::render(ID3D12GraphicsCommandList* command_list)
{
    sample_shader_pass.setup(command_list);

    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    command_list->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    command_list->DrawInstanced(3, 1, 0, 0);
}