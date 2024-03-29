#include "constant_buffers_manager.h"
#include "engine.h"

void constant_buffer_c::init()
{
	auto& engine = engine_c::get_instance();
	d3d_device = engine.get_renderer()->get_d3d_device();
	gpu_allocator = engine.get_renderer()->get_gpu_allocator();
};


HRESULT constant_buffer_c::allocate(const std::wstring name, size_t size)
{
	HRESULT hres;

    buffer_size = ALIGN_CB(size);
	auto buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);

	const std::wstring upload_buffer_name = name + std::wstring(L"_UPLOAD_CB");
	const std::wstring buffer_name = name + std::wstring(L"_CB");

	D3D12MA::ALLOCATION_DESC constant_buffer_desc = {};
	constant_buffer_desc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

	ComPtr<ID3D12Resource> constant_buffer_resource_upload;
	CK(gpu_allocator->CreateResource(
		&constant_buffer_desc,
		&buffer_desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &gpu_upload_buffer, IID_PPV_ARGS(&constant_buffer_resource_upload)));
	constant_buffer_resource_upload->SetName(upload_buffer_name.c_str());

	constant_buffer_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
	ComPtr<ID3D12Resource> constant_buffer_resource;
	CK(gpu_allocator->CreateResource(
		&constant_buffer_desc,
		&buffer_desc,
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
	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(gpu_buffer->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    command_list->CopyResource(gpu_buffer->GetResource(), gpu_upload_buffer->GetResource());
    command_list->ResourceBarrier(1, &transition);
}

HRESULT constant_buffers_manager_c::create_constant_buffer(int& buffer_id, std::wstring name, size_t size)
{
    HRESULT hres;

    constant_buffers_array.emplace_back();
    auto& constant_buffer = constant_buffers_array.back();

	constant_buffer.init();

    CK(constant_buffer.allocate(name, size));

	buffer_id = current_constant_buffer_handle_idx++;

    return S_OK;
}


HRESULT constant_buffers_manager_c::update_constant_buffer(int& buffer_id, BYTE* buffer, size_t size, size_t offset)
{
    auto& constant_buffer = constant_buffers_array[buffer_id];
    return constant_buffer.update_constant_buffer(buffer, size, offset);
}


void constant_buffers_manager_c::copy_buffers_on_gpu(ID3D12GraphicsCommandList* command_list)
{
	for (auto& constant_buffer : constant_buffers_array)
	{
        constant_buffer.copy_buffer_on_gpu(command_list);
	}

    /*HRESULT hres;
    const UINT64 fence = upload_fence_value;
    CK(d3d_direct_queue->Signal(frame_fance.Get(), fence));
    upload_fence_value++;

    if (frame_fance->GetCompletedValue() < fence)
    {
        CK(frame_fance->SetEventOnCompletion(fence, frame_fence_event));
        WaitForSingleObject(frame_fence_event, INFINITE);
    }*/
}
