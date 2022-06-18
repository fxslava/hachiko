#include "constant_buffers_manager.h"
#include "renderer.h"

HRESULT constant_buffers_manager_c::allocate_resources(renderer_i* renderer)
{
    HRESULT hres;
    ComPtr<D3D12MA::Allocator> gpu_allocator(renderer->get_gpu_allocator());
    renderer_c* d3d_renderer = reinterpret_cast<renderer_c*>(renderer);
    d3d_device = d3d_renderer->get_d3d_device();

    D3D12MA::ALLOCATION_DESC constant_buffer_desc = {};
    constant_buffer_desc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

    ComPtr<ID3D12Resource> constant_buffer_resource_upload;
    CK(gpu_allocator->CreateResource(
        &constant_buffer_desc,
        &CD3DX12_RESOURCE_DESC::Buffer(ENGINE_COMMON_CB_SIZE),
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &upload_engine_common_cb, IID_PPV_ARGS(&constant_buffer_resource_upload)));

    constant_buffer_resource_upload->SetName(L"ENGINE_COMMON_CB_UPLOAD");

    constant_buffer_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
    ComPtr<ID3D12Resource> constant_buffer_resource;
    CK(gpu_allocator->CreateResource(
        &constant_buffer_desc,
        &CD3DX12_RESOURCE_DESC::Buffer(ENGINE_COMMON_CB_SIZE),
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, &engine_common_cb, IID_PPV_ARGS(&constant_buffer_resource)));

    constant_buffer_resource->SetName(L"ENGINE_COMMON_CB");

    // Describe and create a render target view (DSV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC cbv_heap_desc = {};
    cbv_heap_desc.NumDescriptors = 1;
    cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    CK(d3d_device->CreateDescriptorHeap(&cbv_heap_desc, IID_PPV_ARGS(&cbv_heap)));

    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = constant_buffer_resource->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = ENGINE_COMMON_CB_SIZE;
    d3d_device->CreateConstantBufferView(&cbvDesc, cbv_heap->GetCPUDescriptorHandleForHeapStart());

    return S_OK;
}


void constant_buffers_manager_c::destroy_resources()
{
    if (upload_engine_common_cb) {
        upload_engine_common_cb->Release();
    }
    if (engine_common_cb) {
        engine_common_cb->Release();
    }
}

HRESULT constant_buffers_manager_c::begin(ENGINE_COMMON_CB*& common_cb)
{
    HRESULT hres;
    const ComPtr<ID3D12Resource> constant_buffer_resource(upload_engine_common_cb->GetResource());

    const CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    CK(constant_buffer_resource->Map(0, &readRange, reinterpret_cast<void**>(&engine_common_cb_ptr)));

    common_cb = engine_common_cb_ptr;

    return S_OK;
}

void constant_buffers_manager_c::end(ID3D12GraphicsCommandList* command_list, D3D12_RANGE& written_range)
{
    const ComPtr<ID3D12Resource> constant_buffer_resource(upload_engine_common_cb->GetResource());
    constant_buffer_resource->Unmap(0, &written_range);

    command_list->CopyResource(engine_common_cb->GetResource(), upload_engine_common_cb->GetResource());
    command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(engine_common_cb->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}