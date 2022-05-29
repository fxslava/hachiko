project(demo_terrain)

include_directories(
    ../engine/platform
    ../engine/api/include
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