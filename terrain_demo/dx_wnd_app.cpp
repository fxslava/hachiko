#include "dx_wnd_app.h"
#include <assert.h>
#include <comdef.h>

#define CK(v) if (FAILED(hres = (v))) return hres


dx_wnd_app_c::dx_wnd_app_c(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show) {
    wnd_app_c::create_window(instance, prev_instance, cmd_line, cmd_show);
}


LRESULT dx_wnd_app_c::on_paint() {
    return 0;
}


void dx_wnd_app_c::on_destroy() {
    destroy_pipeline();
}


HRESULT dx_wnd_app_c::create_pipeline(const D3D_FEATURE_LEVEL feature_level) {
    HRESULT hres;
    CK(create_dxgi_factory());

    auto adapters = enum_adapters(feature_level);
    if (!adapters.empty()) {
        if (FAILED(D3D12CreateDevice(adapters[0], feature_level, IID_PPV_ARGS(&d3d_device))))
            return false;
    }

    dxgi_factory->MakeWindowAssociation(wnd_handle, DXGI_MWA_NO_ALT_ENTER);

    CK(create_command_queues());
    CK(create_swap_chain(swap_chain_frame_num, current_render_target_idx));
    CK(create_rtv_desc_heap(swap_chain_frame_num));
    CK(create_render_target_view(swap_chain_frame_num));
    CK(create_direct_command_list());

    return S_OK;
}


void dx_wnd_app_c::static_render(wnd_app_c* wnd) {
    dx_wnd_app_c* dx_wnd = reinterpret_cast<dx_wnd_app_c*>(wnd);
    HRESULT hres = dx_wnd->on_render();
#ifdef _DEBUG
    if (hres != S_OK) {
        _com_error err(hres);
        OutputDebugString(err.ErrorMessage());
        Sleep(100);
    }
#endif
}


int dx_wnd_app_c::render_loop() {
    return wnd_app_c::loop(this, static_render);
}


HRESULT dx_wnd_app_c::prerecord_render() {
    HRESULT hres;
    CK(d3d_command_allocator->Reset());
    CK(d3d_direct_command_list->Reset(d3d_command_allocator.Get(), d3d_pipeline_state.Get()));

    D3D12_RESOURCE_BARRIER resource_barrier;
    resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    resource_barrier.Transition.pResource = d3d_render_targets[frame_idx].Get();
    resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    d3d_direct_command_list->ResourceBarrier(1, &resource_barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d_rtv_heap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr = SIZE_T(INT64(rtvHandle.ptr) + INT64(frame_idx) * INT64(rtv_desc_size));

    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    d3d_direct_command_list->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    resource_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    resource_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    d3d_direct_command_list->ResourceBarrier(1, &resource_barrier);

    CK(d3d_direct_command_list->Close());
    return S_OK;
}


HRESULT dx_wnd_app_c::on_render() {
    // Record all the commands we need to render the scene into the command list.
    HRESULT hres;
    CK(prerecord_render());

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { d3d_direct_command_list.Get() };
    d3d_direct_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    CK(d3d_swap_chain->Present(1, 0));

    CK(wait_for_prev_frame());
    return S_OK;
}


void dx_wnd_app_c::destroy_pipeline() {
    wait_for_prev_frame();
    CloseHandle(frame_fence_event);
}


HRESULT dx_wnd_app_c::create_dxgi_factory() {
    UINT createFactoryFlags = 0;

#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    HRESULT hres;
    if (FAILED(hres = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgi_factory)))) {
        return hres;
    }

    return S_OK;
}


std::vector <IDXGIAdapter1*> dx_wnd_app_c::enum_adapters(D3D_FEATURE_LEVEL feature_level) {
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


HRESULT dx_wnd_app_c::create_command_queues() {
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
    HRESULT hres;
    if (FAILED(hres = d3d_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3d_direct_queue)))) 
        return hres;
    if (FAILED(hres = d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3d_command_allocator))))
        return hres;
    return S_OK;
}


HRESULT dx_wnd_app_c::create_swap_chain(const UINT frame_num, UINT& frame_idx) {
    assert(frame_num != 0);

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
    if (FAILED(hres = dxgi_factory->CreateSwapChainForHwnd(
        d3d_direct_queue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        wnd_handle,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swap_chain
    ))) {
        return hres;
    }

    swap_chain.As(&d3d_swap_chain);
    frame_idx = d3d_swap_chain->GetCurrentBackBufferIndex();
    return S_OK;
}


HRESULT dx_wnd_app_c::create_rtv_desc_heap(const UINT num_descriptors) {
    assert(num_descriptors != 0);

    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = num_descriptors;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hres;
    if (FAILED(hres = d3d_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&d3d_rtv_heap))))
        return hres;

    rtv_desc_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    return S_OK;
}


HRESULT dx_wnd_app_c::create_render_target_view(const UINT frame_num) {
    assert(rtv_desc_size != 0);
    assert(frame_num != 0);

    // Create frame resources.
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = d3d_rtv_heap->GetCPUDescriptorHandleForHeapStart();

    // Create a RTV for each frame.
    for (UINT i = 0; i < frame_num; i++) {

        ID3D12Resource* render_target = nullptr;
        HRESULT hres;
        CK(d3d_swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_target)));

        d3d_render_targets.push_back(render_target);
        d3d_device->CreateRenderTargetView(render_target, nullptr, rtv_handle);
        rtv_handle.ptr = SIZE_T(INT64(rtv_handle.ptr) + rtv_desc_size);
    }
    return S_OK;
}


HRESULT dx_wnd_app_c::create_direct_command_list() {
    HRESULT hres;
    CK(d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3d_command_allocator.Get(), nullptr, IID_PPV_ARGS(&d3d_direct_command_list)));
    CK(d3d_direct_command_list->Close());
    CK(d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frame_fance)));
    frame_fence_value = 1;
    frame_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (frame_fence_event == nullptr)
        return HRESULT_FROM_WIN32(GetLastError());
    return S_OK;
}


HRESULT dx_wnd_app_c::wait_for_prev_frame()
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
}