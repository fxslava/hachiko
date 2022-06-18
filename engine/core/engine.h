#pragma once
#include "pch.h"
#include "resource_manager.h"

class engine_c
{
public:
    static engine_c& get_instance()
    {
        static engine_c instance;
    	return instance;
    }
private:
    engine_c() = default;
    ~engine_c() = default;

    engine_c(engine_c const&) = delete;
    void operator=(engine_c const&) = delete;


private:
    resource_manager_c resource_manager;
};