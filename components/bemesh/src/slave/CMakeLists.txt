idf_component_register(SRCS "slave.cpp"
                        INCLUDE_DIRS "../../include"
                        REQUIRES nvs_flash bt mid-tier kernel)