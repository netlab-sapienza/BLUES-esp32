# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)



include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS "components/bemesh/src/services" "components/bemesh/src/server"
    $ENV{IDF_PATH}/components/nvs_flash)


project(esp32_ble_mesh)
