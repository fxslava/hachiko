#pragma once
#include <cstdint>
#include <vector>
#include <string>

#define CK(v) if (FAILED(hres = (v))) return hres

void read_binary_file(std::vector<uint8_t> &buffer, const std::string file_name);