#pragma once
#define NOMINMAX
#include <windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

#include "renderer.h"
#include "D3D12MemAlloc.h"
#include "DirectXMath.h"
#include "DirectXCollision.h"
#include "D3D12MemAlloc.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class shader_pass_c
{
public:
	HRESULT create_pso(renderer_c* renderer);
	void setup(ID3D12GraphicsCommandList* command_list);
protected:
	ComPtr<ID3D12RootSignature> root_signature;
	ComPtr<ID3D12PipelineState> pipeline_state;
};