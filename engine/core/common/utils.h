#pragma once
#include <limits>
#include <cstdint>
#include <vector>
#include <string>

#define CK(v) if (FAILED(hres = (v))) return hres

#include <atomic>
#include <concurrent_queue.h>

using namespace concurrency;

template<typename T>
struct RANGE_TEMPL
{
	T min_;
	T max_;

	bool is_empty() const {
		return max_ > min_;
	}

	bool is_intersect(const RANGE_TEMPL<T>& val) const {
		return RANGE_TEMPL(this).intersection(val);
	}

	bool concat(const RANGE_TEMPL<T> &val) const {
		if (max_ == val.min_) {
			max_ =  val.max_;
			return true;
		}
		if (min_ == val.max_) {
			min_ =  val.min_;
			return true;
		}
		return false;
	}

	RANGE_TEMPL<T> merge(const RANGE_TEMPL<T>& val) const {
		RANGE_TEMPL<T> res;
		res.max_ = std::max<T>(max_, val.max_);
		res.min_ = std::min<T>(min_, val.min_);
		return res;
	}

	RANGE_TEMPL<T> intersection(const RANGE_TEMPL<T>& val) const {
		RANGE_TEMPL<T> res;
		res.min_ = std::max<T>(min_, val.min_);
		res.max_ = std::min<T>(max_, val.max_);
		return res;
	}

	RANGE_TEMPL<T> empty() const
	{
		RANGE_TEMPL<T> res;
		res.min_ = std::numeric_limits<T>.max();
		res.max_ = std::numeric_limits<T>.min();
		return res;
	}
};

class ring_buffer_wrapper_c
{
public:
	ring_buffer_wrapper_c(size_t size)
	{

	};

private:
	concurrent_queue<RANGE_TEMPL<int>> fragments;
};

void read_binary_file(std::vector<uint8_t> &buffer, const std::string file_name);