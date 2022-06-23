#pragma once
#include "pch.h"
#include "renderer.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class terrain_render_pass_c
{
public:
	HRESULT create_pso(renderer_c* renderer);
	void setup(ID3D12GraphicsCommandList* command_list);
protected:
	HRESULT create_root_signature(ID3D12Device* device);


	ComPtr<ID3D12RootSignature> root_signature;
	ComPtr<ID3D12PipelineState> pipeline_state;
};