#ifndef __PCH_HEADER
#define __PCH_HEADER

// Platform specific libs
#define NOMINMAX
#include <windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>
#include <wincodec.h>

// STD
#include <unordered_map>
#include <chrono>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <mutex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <thread>
#include <atomic>
#include <concurrent_queue.h>

// Third party libs
#include "d3dx12.h"
#include "D3D12MemAlloc.h"
#include "DirectXMath.h"
#include "DirectXCollision.h"

#endif