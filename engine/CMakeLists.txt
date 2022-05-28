project(hachiko_engine_lib)

include_directories(
	core/
	core/thirdparty/directx/
	core/thirdparty/wsl/
	core/thirdparty/dxguids/
	core/thirdparty/d3d12memalloc/
	api/include/
)

file(
    GLOB SOURCES
	    ./core/input_controller.cpp
        ./core/renderer.cpp
		./core/thirdparty/d3d12memalloc/D3D12MemAlloc.cpp
		./api/src/hachiko_api.cpp
		./api/src/input_controller_api.cpp
)

set(
	CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} /fp:precise"
)

add_library(
	${PROJECT_NAME}
	${SOURCES}
	)

set(
    DEPS_LIBRARIES
        dxgi
		d3d12
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		${DEPS_LIBRARIES}
	)
	
target_include_directories(${PROJECT_NAME} PRIVATE .)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "libs")