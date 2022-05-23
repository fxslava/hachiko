project(hachiko_engine_lib)

file(
    GLOB SOURCES
        ./core/hch_renderer.cpp
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