project(hachiko_engine_lib)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W3 /WX /arch:AVX2)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -pedantic -Werror -mavx2)
endif()

include_directories(
	core/
	core/thirdparty/directx/
	core/thirdparty/wsl/
	core/thirdparty/dxguids/
	core/thirdparty/d3d12memalloc/
	core/thirdparty/directxmath/Inc
	core/thirdparty/directxmath/Extensions
	core/thirdparty/directxmath/SHMath
	api/include/
)

file(
    GLOB SOURCES
	    ./core/input_controller.cpp
        ./core/renderer.cpp
		./core/vec2_math.cpp
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