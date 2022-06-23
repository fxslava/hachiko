project(hachiko_engine_lib)

set(CMAKE_CXX_STANDARD 17)

# Build HLSL shaders
set(HLSL_SHADER_FILES 
		./core/shaders/sample_shader.psh
		./core/shaders/sample_shader.vsh
		./core/shaders/render_terrain_shader.psh
		./core/shaders/render_terrain_shader.vsh
		./core/shaders/render_terrain_shader.hsh
		./core/shaders/render_terrain_shader.dsh
)

add_custom_target(shaders)

set_source_files_properties(core/shaders/sample_shader.vsh PROPERTIES ShaderType "vs")
set_source_files_properties(core/shaders/sample_shader.psh PROPERTIES ShaderType "ps")
set_source_files_properties(core/shaders/render_terrain_shader.vsh PROPERTIES ShaderType "vs")
set_source_files_properties(core/shaders/render_terrain_shader.psh PROPERTIES ShaderType "ps")
set_source_files_properties(core/shaders/render_terrain_shader.hsh PROPERTIES ShaderType "hs")
set_source_files_properties(core/shaders/render_terrain_shader.dsh PROPERTIES ShaderType "ds")
set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES ShaderModel "6_0")

set(SHADERS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/shaders)
file(MAKE_DIRECTORY ${SHADERS_OUTPUT_DIR})

foreach(FILE ${HLSL_SHADER_FILES})
	get_filename_component(FILE_WE ${FILE} NAME_WE)
	get_source_file_property(shadertype ${FILE} ShaderType)
	get_source_file_property(shadermodel ${FILE} ShaderModel)

	add_custom_command(TARGET shaders
	                   COMMAND dxc.exe /nologo /Emain /T${shadertype}_${shadermodel} /Od /O1 /Zi /Fo ${SHADERS_OUTPUT_DIR}/${FILE_WE}_${shadertype}.cso /Fd ${SHADERS_OUTPUT_DIR}/${FILE_WE}_${shadertype}.pdb ${FILE}
	                   MAIN_DEPENDENCY ${FILE}
	                   COMMENT "HLSL ${FILE}"
	                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	                   VERBATIM)
endforeach(FILE)

source_group("Shaders" FILES ${HLSL_SHADER_FILES})

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W3 /WX /arch:AVX2 /DUNICODE)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -pedantic -Werror -mavx2)
endif()

include_directories(
	core/
	core/common
	core/thirdparty/directx
	core/thirdparty/wsl
	core/thirdparty/dxguids
	core/thirdparty/d3d12memalloc
	core/thirdparty/directxmath/Inc
	core/thirdparty/directxmath/Extensions
	core/thirdparty/directxmath/SHMath
	api/include
	platform
)

file(
    GLOB SOURCES
		./platform/wnd_app.cpp
		./core/engine.cpp
	    ./core/input_controller.cpp
		./core/resource_manager.cpp
		./core/constant_buffers_manager.cpp
		./core/wic_resource_loader.cpp
        ./core/renderer.cpp
		./core/terrain.cpp
		./core/shader_pass.cpp
		./core/terrain_render_pass.cpp
		./core/common/utils.cpp
		./core/thirdparty/d3d12memalloc/D3D12MemAlloc.cpp
		./api/src/camera.cpp
		./api/src/hachiko_api.cpp
		./api/src/game_actors.cpp
		./api/src/input_controller_api.cpp
)

set(
	CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} /fp:precise"
)

add_library(
	${PROJECT_NAME}
	${SOURCES}
	${HLSL_SHADER_FILES}
	)
add_dependencies(${PROJECT_NAME} shaders)
target_precompile_headers(
	${PROJECT_NAME}
	PUBLIC
		./core/common/pch.h
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