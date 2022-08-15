#include "wic_resource_loader.h"
#include <cassert>

DXGI_FORMAT dxfi_fomat(const WICPixelFormatGUID& wic_format)
{
    if (wic_format == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
    if (wic_format == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
    if (wic_format == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
    if (wic_format == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
    if (wic_format == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
    if (wic_format == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
    if (wic_format == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
    if (wic_format == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
    if (wic_format == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
    if (wic_format == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
    if (wic_format == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
    if (wic_format == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
    if (wic_format == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
    if (wic_format == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
    if (wic_format == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;
    return DXGI_FORMAT_UNKNOWN;
}


WICPixelFormatGUID can_convert_to_compatible_dxgi_format(WICPixelFormatGUID& wic_format)
{
    if (wic_format == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
    if (wic_format == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    if (wic_format == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    if (wic_format == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    if (wic_format == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    if (wic_format == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
    if (wic_format == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
    if (wic_format == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
    if (wic_format == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
    if (wic_format == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
    if (wic_format == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
    if (wic_format == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
    if (wic_format == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
    if (wic_format == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
    if (wic_format == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
    if (wic_format == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
    if (wic_format == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
    if (wic_format == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
    if (wic_format == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
    if (wic_format == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
    if (wic_format == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    if (wic_format == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    if (wic_format == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    if (wic_format == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    if (wic_format == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    if (wic_format == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
    if (wic_format == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
    if (wic_format == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
    if (wic_format == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
    if (wic_format == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
    if (wic_format == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
    if (wic_format == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
    if (wic_format == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
    if (wic_format == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
    if (wic_format == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
    if (wic_format == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
    if (wic_format == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
    if (wic_format == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
    if (wic_format == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

    return GUID_WICPixelFormatDontCare;
}


// get the number of bits per pixel for a dxgi format
int get_dxgi_bpp(DXGI_FORMAT& dxgi_format)
{
    if (dxgi_format == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
    if (dxgi_format == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
    if (dxgi_format == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
    if (dxgi_format == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
    if (dxgi_format == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
    if (dxgi_format == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
    if (dxgi_format == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;
    if (dxgi_format == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
    if (dxgi_format == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
    if (dxgi_format == DXGI_FORMAT_B5G6R5_UNORM) return 16;
    if (dxgi_format == DXGI_FORMAT_R32_FLOAT) return 32;
    if (dxgi_format == DXGI_FORMAT_R16_FLOAT) return 16;
    if (dxgi_format == DXGI_FORMAT_R16_UNORM) return 16;
    if (dxgi_format == DXGI_FORMAT_R8_UNORM) return 8;
    if (dxgi_format == DXGI_FORMAT_A8_UNORM) return 8;
    return -1;
}

wic_image_c::wic_image_c(wic_image_loader_c& owner, const fs::path& image_path) : loader(owner) {
    load_image(image_path);
}

HRESULT wic_image_c::load_image(const fs::path& image_path) {
    auto& wic_factory = loader.wic_factory;
    ComPtr<IWICBitmapDecoder> wic_decoder;
    ComPtr<IWICBitmapFrameDecode> wic_frame;
    ComPtr<IWICFormatConverter> wic_format_converter;

    HRESULT hres;
    CK(wic_factory->CreateDecoderFromFilename(image_path.generic_wstring().c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wic_decoder));
    CK(wic_decoder->GetFrame(0, &wic_frame));

    ComPtr<IWICBitmap> bitmap;
    CK(wic_factory->CreateBitmapFromSource(wic_frame.Get(), WICBitmapCacheOnDemand, &bitmap));

    CK(bitmap->GetSize(&width, &height));

    WICPixelFormatGUID wic_pixel_format;
    CK(bitmap->GetPixelFormat(&wic_pixel_format));

    dxgi_format = dxfi_fomat(wic_pixel_format);

    bool converted = false;
    if (dxgi_format == DXGI_FORMAT_UNKNOWN)
    {
        const WICPixelFormatGUID compatible_wic_pixel_format = can_convert_to_compatible_dxgi_format(wic_pixel_format);
        assert(compatible_wic_pixel_format != GUID_WICPixelFormatDontCare);

        dxgi_format = dxfi_fomat(compatible_wic_pixel_format);
        CK(wic_factory->CreateFormatConverter(&wic_format_converter));

        BOOL can_convert = FALSE;
        CK(wic_format_converter->CanConvert(wic_pixel_format, compatible_wic_pixel_format, &can_convert));
        assert(can_convert);

        CK(wic_format_converter->Initialize(wic_frame.Get(), compatible_wic_pixel_format, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom));
        converted = true;
    }

    const int bpp = get_dxgi_bpp(dxgi_format);
    bytes_per_row = (width * bpp) / 8;
    const size_t image_size = bytes_per_row * height;

    data.resize(image_size);
    BYTE* image_memory = &data[0];

    if (converted) {
        CK(wic_format_converter->CopyPixels(nullptr, UINT(bytes_per_row), UINT(image_size), image_memory));
    }
    else {
        CK(wic_frame->CopyPixels(nullptr, UINT(bytes_per_row), UINT(image_size), image_memory));
    }

    return S_OK;
}

HRESULT wic_image_loader_c::create_resource_factory(D3D12MA::Allocator* allocator)
{
    d3d_allocator = allocator;

    HRESULT hres;
    CK(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic_factory)));

    D3D12MA::ALLOCATION_DESC alloc_desc = {};
    alloc_desc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
    alloc_desc.CustomPool = nullptr;
    alloc_desc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
    alloc_desc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;

    const D3D12_RESOURCE_DESC upload_buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(RESOURCE_UPLOAD_BUFFER_SIZE_BYTES);

    ID3D12Resource* upload_resource = nullptr;
    CK(d3d_allocator->CreateResource(&alloc_desc, &upload_buffer_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &upload_ring_buffer, IID_PPV_ARGS(&upload_resource)));

    upload_resource->SetName(L"Texture Buffer Upload Resource Heap");

    return S_OK;
}


void wic_image_loader_c::destroy_resource_factory()
{
    if (upload_ring_buffer) {
        upload_ring_buffer->Release();
    }
    /*if (wic_factory) {
        wic_factory->Release();
        wic_factory = nullptr;
    }*/
}

XMINT2 get_texture_size_appropriate_mip(int mip_level, int width, int height) {
    XMINT2 dimensions;
    dimensions.x = width << mip_level;
    dimensions.y = height << mip_level;
    return dimensions;
}

HRESULT wic_image_loader_c::upload_texture_image(
    ID3D12Device* device, 
    ID3D12GraphicsCommandList* command_list, 
    const wic_image_c& image,
    int mip_level_id,
    int layer_id,
    const std::string& resource_name,
    int max_layers,
    int max_mips,
    D3D12MA::Allocation*& resource,
    D3D12_RESOURCE_DESC& desc,
    payload_t& payload)
{
    HRESULT hres;

    if (!resource) {
        XMINT2 texture_dims = get_texture_size_appropriate_mip(mip_level_id, image.get_width(), image.get_height());
        desc = CD3DX12_RESOURCE_DESC::Tex2D(image.get_format(), texture_dims.x, texture_dims.y, max_layers, max_mips);

        D3D12MA::ALLOCATION_DESC alloc_desc = {};
        alloc_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
        alloc_desc.CustomPool = nullptr;
        alloc_desc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
        alloc_desc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;

        ID3D12Resource* texture_resource = nullptr;
        CK(d3d_allocator->CreateResource(&alloc_desc, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, &resource, IID_PPV_ARGS(&texture_resource)));
        texture_resource->SetName(std::wstring(resource_name.begin(), resource_name.end()).c_str());
    }
    
    UINT64 texture_upload_buffer_size = 0;
	UINT numRows;
    UINT64 rowSize;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placed_footprint;

    const int subResourceIndex = mip_level_id + (layer_id * desc.MipLevels);

	device->GetCopyableFootprints(&desc, subResourceIndex, 1, 0, &placed_footprint, &numRows, &rowSize, &texture_upload_buffer_size);

    const auto allocation      = alloc_ring(texture_upload_buffer_size);
    const auto upload_resource = upload_ring_buffer->GetResource();

    const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& subResourceLayout = placed_footprint;
    const uint64_t subResourceHeight = numRows;
    const uint64_t subResourcePitch  = subResourceLayout.Footprint.RowPitch; // align by D3D12_TEXTURE_DATA_PITCH_ALIGNMENT??
    const uint64_t subResourceDepth  = subResourceLayout.Footprint.Depth;
    const uint64_t pitch = std::min(static_cast<uint64_t>(image.get_stride()), subResourcePitch);

    assert(desc.Format == image.get_format());
    assert(subResourceHeight <= image.get_height());
    assert(subResourcePitch >= image.get_height());

    BYTE* upload_memory;
    CD3DX12_RANGE read_range(0, 0);
    CK(upload_resource->Map(0, &read_range, reinterpret_cast<void**>(&upload_memory)));

    BYTE* destinationSubResourceMemory = upload_memory + allocation.gpu_pointer;
    for (uint64_t sliceIndex = 0; sliceIndex < subResourceDepth; sliceIndex++) {
        const BYTE* sourceSubResourceMemory = image.get_image_data();
        for (int height = 0; height < subResourceHeight; height++)
        {
            memcpy(destinationSubResourceMemory, sourceSubResourceMemory, pitch);
            destinationSubResourceMemory += subResourcePitch;
            sourceSubResourceMemory += image.get_stride();
        }
    }

    upload_resource->Unmap(0, nullptr);

    auto dst = CD3DX12_TEXTURE_COPY_LOCATION(resource->GetResource(), subResourceIndex);
    auto src = CD3DX12_TEXTURE_COPY_LOCATION(upload_resource, placed_footprint);

    placed_footprint.Offset = allocation.gpu_pointer;
    command_list->CopyTextureRegion(
        &dst,
        0, 0, 0, 
        &src,
        nullptr);

    payload.allocation = allocation;

    return S_OK;
}