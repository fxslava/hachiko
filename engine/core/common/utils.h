#pragma once
#include <limits>
#include <cstdint>
#include <vector>
#include <string>
#include <list>

#define CK(v) if (FAILED(hres = (v))) return hres

void read_binary_file(std::vector<uint8_t> &buffer, const std::string file_name);