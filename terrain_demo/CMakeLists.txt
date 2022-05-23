project(demo_terrain)

file(
    GLOB APP_SOURCES
        ./demo_terrain.cpp
        ./demo_terrain.rc
        ./wnd_app.cpp
        ./dx_wnd_app.cpp
)

add_executable (demo_terrain WIN32 ${APP_SOURCES})

target_compile_definitions(demo_terrain
  PRIVATE "UNICODE;_UNICODE"
)