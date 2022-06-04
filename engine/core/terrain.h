#pragma once
#include <vector>
#include "game_actors.h"
#include "shader_pass.h"
#include "D3D12MemAlloc.h"
#include "DirectXMath.h"
#include "DirectXCollision.h"
#include "D3D12MemAlloc.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class terrain_base_c : public terrain_i
{
public:
	HRESULT allocate_resources(renderer_i* renderer);
	void render(ID3D12GraphicsCommandList* command_list);
protected:

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	ComPtr<D3D12MA::Allocation> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	shader_pass_c sample_shader_pass;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;
};