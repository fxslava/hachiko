#include "renderer.h"

#define CK(v) if (FAILED(hres = (v))) return hres


HRESULT renderer_c::create_pipeline(const UINT width, const UINT height, UINT num_back_buffer_frames, const D3D_FEATURE_LEVEL feature_level, HWND wnd_handle) {
    screen_resolution = XMINT2(width, height);
    swap_chain_frame_num = num_back_buffer_frames;
    d3d_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    d3d_scissor_rect = CD3DX12_RECT(0, 0, width, height);
    this->wnd_handle = wnd_handle;

    HRESULT hres;

    CK(create_dxgi_factory());

    ComPtr<IDXGIAdapter> preferred_adapter;
    auto adapters = enum_adapters(feature_level);
    if (!adapters.empty()) {
        adapters[0]->QueryInterface(IID_PPV_ARGS(&preferred_adapter));
        CK(D3D12CreateDevice(preferred_adapter.Get(), feature_level, IID_PPV_ARGS(&d3d_device)));
    }

    D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
    allocatorDesc.pDevice = d3d_device.Get();
    allocatorDesc.pAdapter = preferred_adapter.Get();
    CK(D3D12MA::CreateAllocator(&allocatorDesc, &gpu_allocator));

    dxgi_factory->MakeWindowAssociation(wnd_handle, DXGI_MWA_NO_ALT_ENTER);

    CK(create_command_queues());
    CK(create_swap_chain(swap_chain_frame_num, current_render_target_idx));
    CK(create_descriptor_heaps(swap_chain_frame_num));
    CK(create_depth_buffer());
    CK(create_render_target_view(swap_chain_frame_num));
    CK(create_direct_command_lists());
    CK(create_upload_command_list());

    return S_OK;
}


void renderer_c::destroy_pipeline() {
    wait_for_prev_frame();
    d3d_depth_buffer->Release();
    CloseHandle(frame_fence_event);
    gpu_allocator->Release();
}


HRESULT renderer_c::begin_render(ID3D12GraphicsCommandList** command_list)
{
    HRESULT hres;
    CK(render_command_allocator->Reset());
    CK(d3d_render_command_list->Reset(render_command_allocator.Get(), nullptr));
    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(d3d_render_targets[frame_idx].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    d3d_render_command_list->ResourceBarrier(1, &transition);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(d3d_rtv_heap->GetCPUDescriptorHandleForHeapStart(), frame_idx, rtv_desc_size);
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = d3d_dsv_heap->GetCPUDescriptorHandleForHeapStart();
    d3d_render_command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, &dsv_handle);
    *command_list = d3d_render_command_list.Get();
    return hres;
}


HRESULT renderer_c::begin_upload_command_list(ID3D12GraphicsCommandList** command_list)
{
    HRESULT hres;
    CK(copy_command_allocator->Reset());
    CK(d3d_upload_command_list->Reset(copy_command_allocator.Get(), nullptr));

    *command_list = d3d_upload_command_list.Get();
    return hres;
}


HRESULT renderer_c::end_command_list(ID3D12GraphicsCommandList* command_list)
{
    return command_list->Close();
}


HRESULT renderer_c::clear_render_target(float clear_color[4])
{
    D3D12_RECT rect{ 0, 0, screen_resolution.x, screen_resolution.y };

    d3d_render_command_list->RSSetViewports(1, &d3d_viewport);
    d3d_render_command_list->RSSetScissorRects(1, &d3d_scissor_rect);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(d3d_rtv_heap->GetCPUDescriptorHandleForHeapStart(), frame_idx, rtv_desc_size);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsv_handle(d3d_dsv_heap->GetCPUDescriptorHandleForHeapStart());

    d3d_render_command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
    d3d_render_command_list->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    return S_OK;
}


HRESULT renderer_c::end_render(ID3D12GraphicsCommandList* command_list) {
    HRESULT hres;
    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(d3d_render_targets[frame_idx].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    d3d_render_command_list->ResourceBarrier(1, &transition);
    CK(d3d_render_command_list->Close());

    // Execute the command list.
    ID3D12CommandList* pp_render_lists[] = { d3d_render_command_list.Get() };
    d3d_direct_queue->ExecuteCommandLists(_countof(pp_render_lists), pp_render_lists);

    // Present the frame.
    CK(d3d_swap_chain->Present(1, 0));

    CK(wait_for_prev_frame());
    return S_OK;
}


HRESULT renderer_c::create_dxgi_factory() {
    UINT createFactoryFlags = 0;

#if defined(_DEBUG)
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    return CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgi_factory));
}


std::vector <IDXGIAdapter1*> renderer_c::enum_adapters(D3D_FEATURE_LEVEL feature_level) {
    std::vector <IDXGIAdapter1*> vAdapters;
    IDXGIAdapter1* pAdapter = nullptr;

    for (UINT i = 0; dxgi_factory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC1 desc;
        pAdapter->GetDesc1(&desc);

        // skip software adapters
        if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)) {
            if (SUCCEEDED(D3D12CreateDevice(pAdapter, feature_level, _uuidof(ID3D12Device), nullptr))) {
                vAdapters.push_back(pAdapter);
            }
        }
    }

    return vAdapters;
}


HRESULT renderer_c::create_command_queues() {
    HRESULT hres;

    D3D12_COMMAND_QUEUE_DESC direct_queue_desc = {};
    direct_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    direct_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    CK(d3d_device->CreateCommandQueue(&direct_queue_desc, IID_PPV_ARGS(&d3d_direct_queue)));
    CK(d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3d_command_allocator)));
    CK(d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&render_command_allocator)));

    D3D12_COMMAND_QUEUE_DESC upload_queue_desc = {};
    upload_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    upload_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    CK(d3d_device->CreateCommandQueue(&upload_queue_desc, IID_PPV_ARGS(&d3d_upload_queue)));
    CK(d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&copy_command_allocator)));
    return S_OK;
}


HRESULT renderer_c::create_swap_chain(const UINT frame_num, UINT& frame_idx) {
    assert(frame_num != 0);
    assert(wnd_handle != 0);

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = frame_num;
    swapChainDesc.Width = 1920;
    swapChainDesc.Height = 1080;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    HRESULT hres;
    ComPtr<IDXGISwapChain1> swap_chain;
    CK(dxgi_factory->CreateSwapChainForHwnd(
        d3d_direct_queue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        wnd_handle,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swap_chain
    ));

    swap_chain.As(&d3d_swap_chain);
    frame_idx = d3d_swap_chain->GetCurrentBackBufferIndex();
    return S_OK;
}


HRESULT renderer_c::create_depth_buffer()
{
    HRESULT hres;
    D3D12_CLEAR_VALUE depthOptimizedClearValue;
    depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    D3D12MA::ALLOCATION_DESC depth_buffer_desc = {};
    depth_buffer_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

    auto depth_text2d_desc = CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        screen_resolution.x,
        screen_resolution.y,
        1, 0, 1, 0,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    ComPtr<ID3D12Resource> d3d_depth_buffer_resource;
    CK(gpu_allocator->CreateResource(
        &depth_buffer_desc,
        &depth_text2d_desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, &d3d_depth_buffer, IID_PPV_ARGS(&d3d_depth_buffer_resource)));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvView = {};
    dsvView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvView.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvView.Flags = D3D12_DSV_FLAG_NONE;

    d3d_device->CreateDepthStencilView(d3d_depth_buffer_resource.Get(), &dsvView, d3d_dsv_heap->GetCPUDescriptorHandleForHeapStart());
    return S_OK;
}


HRESULT renderer_c::create_descriptor_heaps(const UINT num_descriptors) {
    assert(num_descriptors != 0);

    HRESULT hres;

    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
    rtv_heap_desc.NumDescriptors = num_descriptors;
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    CK(d3d_device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&d3d_rtv_heap)));

    // Describe and create a render target view (DSV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
    dsv_heap_desc.NumDescriptors = 1;
    dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    CK(d3d_device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(&d3d_dsv_heap)));

    rtv_desc_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    return S_OK;
}


HRESULT renderer_c::create_render_target_view(const UINT frame_num) {
    assert(rtv_desc_size != 0);
    assert(frame_num != 0);

    // Create frame resources.
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(d3d_rtv_heap->GetCPUDescriptorHandleForHeapStart());

    // Create a RTV for each frame.
    for (UINT i = 0; i < frame_num; i++) {

        ID3D12Resource* render_target = nullptr;
        HRESULT hres;
        CK(d3d_swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_target)));

        d3d_render_targets.push_back(render_target);
        d3d_device->CreateRenderTargetView(render_target, nullptr, rtv_handle);
        rtv_handle.Offset(1, rtv_desc_size);
    }
    return S_OK;
}


HRESULT renderer_c::create_direct_command_lists() {
    HRESULT hres;
    CK(d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, render_command_allocator.Get(), nullptr, IID_PPV_ARGS(&d3d_render_command_list)));
    CK(d3d_render_command_list->Close());

    CK(d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frame_fance)));
    frame_fence_value = 1;
    frame_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (frame_fence_event == nullptr)
        return HRESULT_FROM_WIN32(GetLastError());
    return S_OK;
}


HRESULT renderer_c::create_upload_command_list() {
    HRESULT hres;
    CK(d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, copy_command_allocator.Get(), nullptr, IID_PPV_ARGS(&d3d_upload_command_list)));
    CK(d3d_upload_command_list->Close());
    return S_OK;
}


HRESULT renderer_c::wait_for_prev_frame()
{
    HRESULT hres;
    const UINT64 fence = frame_fence_value;
    CK(d3d_direct_queue->Signal(frame_fance.Get(), fence));
    frame_fence_value++;

    if (frame_fance->GetCompletedValue() < fence)
    {
        CK(frame_fance->SetEventOnCompletion(fence, frame_fence_event));
        WaitForSingleObject(frame_fence_event, INFINITE);
    }

    frame_idx = d3d_swap_chain->GetCurrentBackBufferIndex();
    return S_OK;
}