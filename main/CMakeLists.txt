idf_component_register(SRCS "esp32_ble_mesh.cpp"
                        INCLUDE_DIRS "../components/bemesh/include"
                        REQUIRES kernel mid-tier callbacks)