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

HRESULT wic_image_loader_c::load_texture(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, const fs::path& texture_path, const std::wstring& resource_name, payload_t& payload, D3D12_RESOURCE_DESC& resource_desc, D3D12MA::Allocation*& texture)
{
    ComPtr<IWICBitmapDecoder> wic_decoder;
    ComPtr<IWICBitmapFrameDecode> wic_frame;
    ComPtr<IWICFormatConverter> wic_format_converter;

    HRESULT hres;
    CK(wic_factory->CreateDecoderFromFilename(texture_path.generic_wstring().c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wic_decoder));
    CK(wic_decoder->GetFrame(0, &wic_frame));

    ComPtr<IWICBitmap> bitmap;
    CK(wic_factory->CreateBitmapFromSource(wic_frame.Get(), WICBitmapCacheOnDemand, &bitmap));

    UINT width, height;
    CK(bitmap->GetSize(&width, &height));

    WICPixelFormatGUID wic_pixel_format;
    CK(bitmap->GetPixelFormat(&wic_pixel_format));

    DXGI_FORMAT dxgi_format = dxfi_fomat(wic_pixel_format);

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
    const size_t bytes_per_row = (width * bpp) / 8;
    const size_t image_size = bytes_per_row * height;

    std::vector<BYTE> memory_image(image_size);
    BYTE* image_memory = &memory_image[0];

    if (converted) {
        CK(wic_format_converter->CopyPixels(nullptr, UINT(bytes_per_row), UINT(image_size), image_memory));
    } else {
        CK(wic_frame->CopyPixels(nullptr, UINT(bytes_per_row), UINT(image_size), image_memory));
    }

    const D3D12_RESOURCE_DESC texture_desc = CD3DX12_RESOURCE_DESC::Tex2D(dxgi_format, width, height);
    const int num_of_subresources = (texture_desc.MipLevels + 1) * texture_desc.DepthOrArraySize;
    
    UINT64 texture_upload_buffer_size = 0;
	std::vector<UINT> numRows(texture_desc.MipLevels + 1);
    std::vector<UINT64> rowSize(texture_desc.MipLevels + 1);
	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> placed_footprint(num_of_subresources);

	device->GetCopyableFootprints(&texture_desc, 0, num_of_subresources, 0, placed_footprint.data(), numRows.data(), rowSize.data(), &texture_upload_buffer_size);

    const auto allocation      = alloc_ring(texture_upload_buffer_size);
    const auto upload_resource = upload_ring_buffer->GetResource();

    for (int arrayIndex = 0; arrayIndex < texture_desc.DepthOrArraySize; arrayIndex++) {
        for (int mipIndex = 0; mipIndex < texture_desc.MipLevels + 1; mipIndex++) {
            const int subResourceIndex = mipIndex + (arrayIndex * texture_desc.MipLevels);

            const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& subResourceLayout = placed_footprint[subResourceIndex];
            const uint64_t subResourceHeight = numRows[subResourceIndex];
            const uint64_t subResourcePitch  = subResourceLayout.Footprint.RowPitch; // align by D3D12_TEXTURE_DATA_PITCH_ALIGNMENT??
            const uint64_t subResourceDepth  = subResourceLayout.Footprint.Depth;
            const uint64_t pitch = std::min(bytes_per_row, subResourcePitch);

            BYTE* upload_memory;
            CD3DX12_RANGE read_range(0, 0);
            CK(upload_resource->Map(subResourceIndex, &read_range, reinterpret_cast<void**>(&upload_memory)));

            BYTE* destinationSubResourceMemory = upload_memory + subResourceLayout.Offset + allocation.gpu_pointer;

            for (uint64_t sliceIndex = 0; sliceIndex < subResourceDepth; sliceIndex++) {

                const BYTE* sourceSubResourceMemory = image_memory;
                for (int height = 0; height < subResourceHeight; height++)
                {
                    memcpy(destinationSubResourceMemory, sourceSubResourceMemory, pitch);
                    destinationSubResourceMemory += subResourcePitch;
                    sourceSubResourceMemory += bytes_per_row;
                }
            }

            upload_resource->Unmap(subResourceIndex, nullptr);
        }
    }

    D3D12MA::ALLOCATION_DESC alloc_desc = {};
    alloc_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
    alloc_desc.CustomPool = nullptr;
    alloc_desc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
    alloc_desc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;

    ID3D12Resource* texture_resource = nullptr;
    CK(d3d_allocator->CreateResource(&alloc_desc, &texture_desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, &texture, IID_PPV_ARGS(&texture_resource)));
    texture_resource->SetName(resource_name.c_str());

    for (int i = 0; i < placed_footprint.size(); i++) {
        command_list->CopyTextureRegion(
            &CD3DX12_TEXTURE_COPY_LOCATION(texture_resource, i), 
            0, 0, 0, 
            &CD3DX12_TEXTURE_COPY_LOCATION(upload_resource, placed_footprint[i]), 
            nullptr);
    }

    resource_desc = texture_desc;
    payload.allocation = allocation;

    return S_OK;
}