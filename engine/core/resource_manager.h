#pragma once
#include "pch.h"

#include "utils.h"
#include "renderer.h"
#include "wic_resource_loader.h"

using namespace Concurrency;
using Microsoft::WRL::ComPtr;
namespace fs = std::filesystem;

constexpr int RESOURCE_LOADING_CONCURRENT_TASKS_MAX_NUM = 128;
constexpr int RESOURCE_UPLOAD_CHUNK_SIZE = 8;
constexpr int RESOURCE_MAX_MIP_MAPS = 8;
constexpr int RESOURCE_MAX_LAYERS = 256;

class resource_manager_c
{
public:
	using RESOURCE_ID = int32_t;
	using SUBRESOURCE_ID = int32_t;

	enum RESOURCE_TYPE {
		UNDEFINED,
		TEXTURE
	};

	enum RESOURCE_STATE {
		RESOURCE_NOT_EXIST,
		RESOURCE_NOT_LOADED,
		RESOURCE_REQUESTED,
		RESOURCE_AVAILABLE
	};

	enum QUERY_RESOURCE_STATE {
		NOT_EXIST,
		LOADING,
		AVAILABLE,
		BUSY
	};

	struct GPU_SUBRESOURCE {
		fs::path file_path;
		int layer_id = 0;
		int mip_id = 0;
	};

	struct GPU_RESOURCE {
		RESOURCE_STATE state = RESOURCE_NOT_EXIST;
		D3D12MA::Allocation* resource = nullptr;
		D3D12_RESOURCE_DESC desc{};
		std::list<int> subresources;
		std::string name;
		int max_num_layers = 1;
		int max_num_mip_maps = 1;
	};

	struct UPLOAD_TASK {
		D3D12MA::Allocation* upload_buffer;
		RESOURCE_ID resource_id;
	};

	void init();
	void init(const fs::path& root);
	void start();
	void terminate();
	HRESULT create_resource_factory(renderer_c* renderer);
	void destroy_resource_factory();
	HRESULT wait_for_resources_uploaded();
	QUERY_RESOURCE_STATE query_resource(RESOURCE_ID resource_id);			// thread safe
	QUERY_RESOURCE_STATE query_resource(const std::string& resource_id);    // thread safe
	GPU_RESOURCE* get_resource(RESOURCE_ID resource_id);
	GPU_RESOURCE* get_resource(const std::string& resource_id);

private:
	void release_upload_list(std::list<UPLOAD_TASK>& upload_list);

	struct RESOURCE_DESC_FROM_NAME {
		int32_t layer_id = 0;
		int32_t lod_id = 0;
	};

	RESOURCE_DESC_FROM_NAME get_desc_from_resource_name(std::string resource_name);

	wic_image_loader_c wic_image_loader;
	ComPtr<ID3D12CommandQueue> upload_queue;

	struct LOAD_QUERY
	{
		LOAD_QUERY() {}
		LOAD_QUERY(RESOURCE_TYPE type_, fs::path file_name_) : type(type_) {};

		RESOURCE_TYPE type = TEXTURE;
		RESOURCE_ID resource_id = -1;
		int mip_id = 0;
		int layer_id = 0;
	};
	mutable LOAD_QUERY* TERMINATE_QUERY = nullptr;

	bool is_available(const std::string& resource_name);

	void resource_manager_scheduler();
	static void static_resource_manager_scheduler(resource_manager_c* owner) {
		owner->resource_manager_scheduler();
	}

	std::thread *scheduler = nullptr;
	std::unordered_map<std::string, RESOURCE_ID> resource_registry;
	std::unordered_map<std::string, SUBRESOURCE_ID> subresource_registry;
	std::vector<GPU_RESOURCE> gpu_resources;
	std::vector<GPU_SUBRESOURCE> gpu_subresources;

	std::mutex resource_lock;
	std::atomic<int> in_progress_tasks_num = 0;
	std::array<LOAD_QUERY, RESOURCE_LOADING_CONCURRENT_TASKS_MAX_NUM> load_tasks_pool;
	concurrent_queue<LOAD_QUERY*> load_tasks;

	renderer_c* d3d_renderer = nullptr;
	D3D12MA::Allocator* d3d_allocator = nullptr;

	HANDLE resource_fence_event = 0;
	UINT64 resource_fence_value = 0;
	ComPtr<ID3D12Fence> resource_fance;
};