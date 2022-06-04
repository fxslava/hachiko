#include "shader_pass.h"
#include "utils.h"

HRESULT shader_pass_c::create_pso(renderer_c* renderer)
{
    HRESULT hres = S_OK;
	ComPtr<ID3D12Device> d3d_device(renderer->get_d3d_device());
	ComPtr<D3D12MA::Allocator> gpu_allocator(renderer->get_gpu_allocator());

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    CK(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
    CK(d3d_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature)));

    std::vector<uint8_t> vertex_shader_bytecode;
    read_binary_file(vertex_shader_bytecode, "shaders/sample_shader_vs.cso");
    std::vector<uint8_t> pixel_shader_bytecode;
    read_binary_file(pixel_shader_bytecode, "shaders/sample_shader_ps.cso");

    // Define the vertex input layout.
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = root_signature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(&vertex_shader_bytecode[0], vertex_shader_bytecode.size());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(&pixel_shader_bytecode[0], pixel_shader_bytecode.size());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    CK(d3d_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipeline_state)));

    return hres;
}


void shader_pass_c::setup(ID3D12GraphicsCommandList* command_list)
{
    command_list->SetGraphicsRootSignature(root_signature.Get());
    command_list->SetPipelineState(pipeline_state.Get());
}