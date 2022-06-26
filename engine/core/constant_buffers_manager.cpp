#include "constant_buffers_manager.h"
#include "renderer.h"


HRESULT constant_buffer_c::allocate(const std::wstring name, size_t size)
{
	HRESULT hres;

    buffer_size = ALIGN_CB(size);

	const std::wstring upload_buffer_name = name + std::wstring(L"_UPLOAD_CB");
	const std::wstring buffer_name = name + std::wstring(L"_CB");

	D3D12MA::ALLOCATION_DESC constant_buffer_desc = {};
	constant_buffer_desc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

	ComPtr<ID3D12Resource> constant_buffer_resource_upload;
	CK(gpu_allocator->CreateResource(
		&constant_buffer_desc,
		&CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &gpu_upload_buffer, IID_PPV_ARGS(&constant_buffer_resource_upload)));
	constant_buffer_resource_upload->SetName(upload_buffer_name.c_str());

	constant_buffer_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
	ComPtr<ID3D12Resource> constant_buffer_resource;
	CK(gpu_allocator->CreateResource(
		&constant_buffer_desc,
		&CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
		D3D12_RESOURCE_STATE_COPY_DEST, nullptr, &gpu_buffer, IID_PPV_ARGS(&constant_buffer_resource)));
	constant_buffer_resource->SetName(buffer_name.c_str());

	return S_OK;
}

HRESULT constant_buffer_c::update_constant_buffer(BYTE* buffer, size_t size, size_t offset) const
{
	const ComPtr<ID3D12Resource> constant_buffer_resource(gpu_upload_buffer->GetResource());
	const CD3DX12_RANGE readRange(0, 0);

	BYTE* dst = nullptr;
	if (SUCCEEDED(constant_buffer_resource->Map(0, &readRange, reinterpret_cast<void**>(&dst)))) {
		memcpy(dst + offset, buffer, size);
		constant_buffer_resource->Unmap(0, nullptr);
        return S_OK;
	}

	return E_FAIL;
}


void constant_buffer_c::copy_buffer_on_gpu(ID3D12GraphicsCommandList* command_list)
{
    command_list->CopyResource(gpu_buffer->GetResource(), gpu_upload_buffer->GetResource());
    command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gpu_buffer->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
}


HRESULT constant_buffers_manager_c::create_constant_buffer(CONSTANT_BUFFER_HANDLE& cb_handle, std::wstring name, size_t size)
{
    HRESULT hres;

    constant_buffers_array.emplace_back(d3d_renderer);
    auto& constant_buffer = constant_buffers_array.back();

    CK(constant_buffer.allocate(name, size));

    cb_handle = current_constant_buffer_handle_idx++;

    return S_OK;
}


HRESULT constant_buffers_manager_c::update_constant_buffer(CONSTANT_BUFFER_HANDLE& cb_handle, BYTE* buffer, size_t size, size_t offset)
{
    auto& constant_buffer = constant_buffers_array[cb_handle];
    return constant_buffer.update_constant_buffer(buffer, size, offset);
}


HRESULT constant_buffers_manager_c::get_cbv_heap(std::vector<CONSTANT_BUFFER_HANDLE>& cb_handle_array, ID3D12DescriptorHeap*& cbv_heap)
{
    const CBV_HEAP_ID heap_id(cb_handle_array);
    if (!is_cbv_heap_available_in_cache(heap_id)) {
        return create_cbv_heap(heap_id, cb_handle_array, cbv_heap);
    }
    cbv_heap = cbv_heap_cache[heap_id.ID_ARRAY.raw];
    return S_OK;
}

HRESULT constant_buffers_manager_c::create_cbv_heap(const CBV_HEAP_ID& heap_id, std::vector<CONSTANT_BUFFER_HANDLE>& cb_handle_array, ID3D12DescriptorHeap*& cbv_heap)
{
    HRESULT hres;

    D3D12_DESCRIPTOR_HEAP_DESC cbv_heap_desc = {};
    cbv_heap_desc.NumDescriptors = static_cast<UINT>(cb_handle_array.size());
    cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    CK(d3d_device->CreateDescriptorHeap(&cbv_heap_desc, IID_PPV_ARGS(&cbv_heap)));

    cbv_heap_cache[heap_id.ID_ARRAY.raw] = cbv_heap;

    CD3DX12_CPU_DESCRIPTOR_HANDLE cbv_heap_handle(cbv_heap->GetCPUDescriptorHandleForHeapStart());

    for (const auto& handle : cb_handle_array) {
        auto& constant_buffer = constant_buffers_array[handle];

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
        cbvDesc.BufferLocation = constant_buffer.GetGPUVirtualAddress();
        cbvDesc.SizeInBytes    = static_cast<UINT>(constant_buffer.get_size());
        d3d_device->CreateConstantBufferView(&cbvDesc, cbv_heap_handle);

        cbv_heap_handle.Offset(d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    }
    return S_OK;
}


void constant_buffers_manager_c::copy_buffers_on_gpu(ID3D12GraphicsCommandList* command_list)
{
	for (auto& constant_buffer : constant_buffers_array)
	{
        constant_buffer.copy_buffer_on_gpu(command_list);
	}
}
