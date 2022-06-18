project(demo_terrain)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    ../engine/platform
    ../engine/api/include
    ../engine/core/
    ../engine/core/common
    ../engine/core/thirdparty/d3d12memalloc
    ../engine/core/thirdparty/directxmath/inc
    ../engine/core/thirdparty/directx
)

file(
    GLOB APP_SOURCES
        ./demo_terrain.cpp
        ./demo_terrain.rc
        ./demo_terrain_app.cpp
)

add_executable (
    ${PROJECT_NAME} 
    WIN32 
        ${APP_SOURCES}
    )

set(
    DEPS_LIBRARIES
        hachiko_engine_lib
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		${DEPS_LIBRARIES}
	)

target_compile_definitions(demo_terrain
  PRIVATE "UNICODE;_UNICODE"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")