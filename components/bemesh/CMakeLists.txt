set(COMPONENT_SRCS "src/bemesh.cpp" "src/bleClient/*.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "include")
register_component()

add_executable(bleClientTest "src/bleClient/main_test.cpp")