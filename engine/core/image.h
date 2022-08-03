#pragma once
#include "pch.h"

class image_c {
public:
	int get_width() const { return width; }
	int get_height() const { return height; }
	int get_stride() const { return bytes_per_row; }
	const BYTE* get_image_data() const { return data.data(); }
	DXGI_FORMAT get_format() const { return dxgi_format; }
protected:
	std::vector<BYTE> data{};
	uint32_t width = 0;
	uint32_t height = 0;
	int bytes_per_row = 0;
	DXGI_FORMAT dxgi_format = DXGI_FORMAT_UNKNOWN;
};

class texture_c {
public:

protected:
	int depth_or_array_size = 1;
	int mip_levels = 1;
	int width = 0;
	int height = 0;
	int depth = 0;
};
