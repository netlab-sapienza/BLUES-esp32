# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)



include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS "components/bemesh/src/kernel" "components/bemesh/src/services" "components/bemesh/src/master"
     $ENV{IDF_PATH}/components/nvs_flash "components/bemesh/src/slave"
     "components/bemesh/src/mid-tier" "components/bemesh/src/callbacks"  )


project(esp32_ble_mesh)
