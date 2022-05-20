project(demo_terrain)

add_executable (demo_terrain WIN32 "demo_terrain.cpp" "demo_terrain.rc")

target_compile_definitions(demo_terrain
  PRIVATE "UNICODE;_UNICODE"
)