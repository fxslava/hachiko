#include "gpu_heaps_manager.h"
#include "engine.h"

HRESULT cpu_heap_c::create(int num_descriptors_, D3D12_DESCRIPTOR_HEAP_TYPE heap_type) {
    auto& engine = engine_c::get_instance();
    d3d_renderer = engine.get_renderer();
    d3d_device = d3d_renderer->get_d3d_device();

    HRESULT hres;
    num_descriptors = num_descriptors_;

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = num_descriptors;
    srvHeapDesc.Type = heap_type;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    CK(d3d_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srv_heap)));

    available_descriptors = num_descriptors;
    descriptor_size = d3d_device->GetDescriptorHandleIncrementSize(heap_type);
    current_cpu_descriptor_handle = srv_heap->GetCPUDescriptorHandleForHeapStart();

    return S_OK;
}


bool cpu_heap_c::allocate(DESCRIPTOR_HEAP_ID& desc_id, int num_descriptors) {
    assert(srv_heap);
    if (available_descriptors >= num_descriptors) {
        current_cpu_descriptor_handle.Offset(desc_id.descriptor_idx, descriptor_size);
        desc_id.handle = current_cpu_descriptor_handle;
        desc_id.descriptor_idx = current_descriptor_id;
        current_descriptor_id += num_descriptors;
        available_descriptors -= num_descriptors;
        return true;
    }
    return false;
}


void cpu_heap_c::reset() {
    assert(srv_heap);
    current_descriptor_id = 0;
    available_descriptors = num_descriptors;
}


HRESULT gpu_heap_c::create(int num_descriptors_, D3D12_DESCRIPTOR_HEAP_TYPE heap_type_) {
    auto& engine = engine_c::get_instance();
    d3d_renderer = engine.get_renderer();
    d3d_device = d3d_renderer->get_d3d_device();

    num_descriptors = num_descriptors_;
    heap_type = heap_type_;
    descriptor_size = d3d_device->GetDescriptorHandleIncrementSize(heap_type);
    return create_heap(num_descriptors, &srv_heap);
}


HRESULT gpu_heap_c::resize(int new_descriptors_num) {
    HRESULT hres;

    ID3D12DescriptorHeap* p_heap = nullptr;
    CK(create_heap(num_descriptors, &p_heap));

    srv_heap.Reset();
    srv_heap = p_heap;

    return S_OK;
}


HRESULT gpu_heap_c::flush_heaps(int offset, std::vector<ID3D12DescriptorHeap*> heaps) {

    CD3DX12_CPU_DESCRIPTOR_HANDLE dst_handle(srv_heap->GetCPUDescriptorHandleForHeapStart(), offset, descriptor_size);

    for (auto& heap : heaps) {
        const D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();

        d3d_device->CopyDescriptorsSimple(
            desc.NumDescriptors,
            dst_handle,
            heap->GetCPUDescriptorHandleForHeapStart(),
            heap_type);

        dst_handle.Offset(desc.NumDescriptors, descriptor_size);
    }

    return S_OK;
}


HRESULT gpu_heap_c::create_heap(int new_descriptors_num, ID3D12DescriptorHeap** heap) {
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = new_descriptors_num;
    srvHeapDesc.Type = heap_type;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    return d3d_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(heap));
}


void gpu_heap_allocator_c::init(int _descriptors_chunk_size, D3D12_DESCRIPTOR_HEAP_TYPE _heap_type) {
    descriptors_chunk_size = _descriptors_chunk_size;
    heap_type = _heap_type;
}


void gpu_heap_allocator_c::reset() {
    current_descriptor_id.descriptor_idx = 0;
    for (auto& heap : dynamic_heaps) {
        heap.reset();
    }
}


DESCRIPTOR_HEAP_ID gpu_heap_allocator_c::dynamic_alloc(int num_descriptors) {
    DESCRIPTOR_HEAP_ID handle = current_descriptor_id;
    handle.static_descriptor = false;

    current_descriptor_id.descriptor_idx += num_descriptors;

    auto& heaps = dynamic_heaps;

    if (heaps.empty()) {
        heaps.emplace_back().create(std::max(descriptors_chunk_size, num_descriptors), heap_type);
    }

    for (auto& heap : heaps) {
        if (heap.allocate(handle, num_descriptors)) {
            return handle;
        }
    }

    auto& new_heap = heaps.emplace_back();
    new_heap.create(std::max(descriptors_chunk_size, num_descriptors), heap_type);
    new_heap.allocate(handle, num_descriptors);
    return handle;
}


void gpu_heap_allocator_c::get_heap_pool(ID3D12DescriptorHeap** p_heaps) {
    for (auto& heap : dynamic_heaps) {
        *p_heaps = heap.get_heap();
        p_heaps++;
    }
}


HRESULT gpu_heaps_manager_c::create(int num_back_buffer_frames_, int descriptors_chunk_size) {
    HRESULT hres;
    auto& engine = engine_c::get_instance();
    d3d_renderer = engine.get_renderer();
    d3d_device = d3d_renderer->get_d3d_device();

    num_back_buffer_frames = num_back_buffer_frames_;

    dynamic_heap_pool.init(descriptors_chunk_size);
    CK(gpu_heap.create(GPU_HEAP_DEFAULT_DESCRIPTORS_COUNT));

    descriptor_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    return S_OK;
}


DESCRIPTOR_HEAP_ID gpu_heaps_manager_c::dynamic_alloc(int num_descriptors) {
    return dynamic_heap_pool.dynamic_alloc(num_descriptors);
}

DESCRIPTOR_MANAGER_HANDLE gpu_heaps_manager_c::static_alloc(int num_descriptors) {
    DESCRIPTOR_MANAGER_HANDLE handle;

    auto heap = gpu_heap.get_descriptor_heap();
    handle.cpu_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart(), current_descriptor_id.descriptor_idx, descriptor_size);
    handle.gpu_handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(heap->GetGPUDescriptorHandleForHeapStart(), current_descriptor_id.descriptor_idx, descriptor_size);
    handle.p_heap = heap;

    current_descriptor_id.descriptor_idx += num_descriptors;
    return handle;
}


void gpu_heaps_manager_c::assembly_heaps_current_list() {
    size_t total_heaps_num = dynamic_heap_pool.get_heaps_count();

    if (!total_heaps_num)
        return;

    current_heaps.resize(total_heaps_num);
    ID3D12DescriptorHeap** p_current_heaps = current_heaps.data();

    dynamic_heap_pool.get_heap_pool(p_current_heaps);
    p_current_heaps += dynamic_heap_pool.get_heaps_count();
}


void gpu_heaps_manager_c::next_frame() {
    assembly_heaps_current_list();

    gpu_heap.flush_heaps(current_descriptor_id.descriptor_idx, current_heaps);

    dynamic_heap_pool.reset();

    current_frame_idx = (current_frame_idx + 1) % num_back_buffer_frames;
}