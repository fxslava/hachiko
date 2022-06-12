#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <thread>
#include <iostream>
#include <filesystem>

#define NOMINMAX
#include <windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>
#include <wincodec.h>

#include <mutex>
#include <atomic>
#include <concurrent_queue.h>

#include "utils.h"
#include "renderer.h"
#include "wic_resource_loader.h"

using namespace Concurrency;
using Microsoft::WRL::ComPtr;
namespace fs = std::filesystem;

constexpr int RESOURCE_LOADING_CONCURRENT_TASKS_MAX_NUM = 128;
constexpr int RESOURCE_UPLOAD_CHUNK_SIZE = 16;

class resource_manager_c
{
public:
	~resource_manager_c();

	enum RESOURCE_ENTITY_TYPE
	{
		UNDEFINED,
		TEXTURE
	};

	enum RESOURCE_STATE
	{
		RESOURCE_NOT_EXIST,
		RESOURCE_NOT_LOADED,
		RESOURCE_REQUESTED,
		RESOURCE_AVAILABLE
	};

	enum QUERY_RESOURCE_STATE
	{
		NOT_EXIST,
		LOADING,
		AVAILABLE,
		BUSY
	};

	struct RESOURCE_ENTITY
	{
		RESOURCE_ENTITY_TYPE type = UNDEFINED;
		RESOURCE_STATE state = RESOURCE_NOT_EXIST;
		fs::path resource_path = "";
	};

	using RESOURCE_ID = int32_t;

	void init();
	void init(const fs::path& root);
	void start();
	void terminate();
	HRESULT create_resource_factory(renderer_c* renderer);
	HRESULT wait_for_resources_uploaded();
	QUERY_RESOURCE_STATE query_resource(RESOURCE_ID resource_id);			 // thread safe
	QUERY_RESOURCE_STATE query_resource(const std::wstring& resource_id);    // thread safe

	ID3D12Resource* get_resource(RESOURCE_ID resource_id)
	{
		if (resources[resource_id].state == RESOURCE_AVAILABLE) {
			return gpu_resources[resource_id]->GetResource();
		}

		return nullptr;
	}

	ID3D12Resource* get_resource(const std::wstring& resource_id)
	{
		if (!is_available(resource_id)) {
			return nullptr;
		}

		return get_resource(recource_registry[resource_id]);
	}

	HRESULT update();

private:
	ComPtr<ID3D12CommandQueue> upload_queue;

	struct LOAD_QUERY
	{
		LOAD_QUERY() {}
		LOAD_QUERY(RESOURCE_ENTITY_TYPE type_, fs::path file_name_) : type(type_), resource_path(file_name_) {};

		RESOURCE_ENTITY_TYPE type = TEXTURE;
		fs::path resource_path = "";
		RESOURCE_ID resource_id = -1;
	};
	mutable LOAD_QUERY* TERMINATE_QUERY = nullptr;

	bool is_available(const std::wstring& resource_id);

	void resource_manager_scheduler();
	static void static_resource_manager_scheduler(resource_manager_c* owner) {
		owner->resource_manager_scheduler();
	}

	std::thread *scheduler = nullptr;
	std::unordered_map<std::wstring, RESOURCE_ID> recource_registry;
	std::vector<RESOURCE_ENTITY> resources;
	std::vector<D3D12MA::Allocation*> gpu_resources;
	std::vector<std::wstring> resource_names;

	std::mutex resource_lock;
	std::atomic<int> in_progress_tasks_num = 0;
	std::array<LOAD_QUERY, RESOURCE_LOADING_CONCURRENT_TASKS_MAX_NUM> load_tasks_pool;
	concurrent_queue<LOAD_QUERY*> load_tasks;
	concurrent_queue<RESOURCE_ID*> upload_tasks;

	renderer_c* d3d_renderer = nullptr;
	D3D12MA::Allocator* d3d_allocator = nullptr;

	HANDLE resource_fence_event = 0;
	UINT64 resource_fence_value = 0;
	ComPtr<ID3D12Fence> resource_fance;

	struct resource_payload_t
	{
		wic_image_loader_c::payload_t texture_payload;
		RESOURCE_ID resource_id;
	};

	wic_image_loader_c wic_image_loader;
};