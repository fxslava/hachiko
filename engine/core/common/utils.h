#pragma once
#include <limits>
#include <cstdint>
#include <vector>
#include <string>
#include <list>

#define CK(v) if (FAILED(hres = (v))) return hres
#define CLAMP(v, min_v, max_v) ((v) < (min_v) ? (min_v) : ((v) > (max_v) ? (max_v) : (v)))

void read_binary_file(std::vector<uint8_t> &buffer, const std::string file_name);