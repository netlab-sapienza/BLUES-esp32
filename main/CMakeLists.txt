
list(APPEND REQUIRED_COMPS "kernel" "mid-tier" "callbacks" "common" "firmware")


#In this variable we keep track of source files needed by the component.
list(APPEND SOURCES "esp32_ble_mesh.cpp")


#All header files will be located inside the "include" directory of the component "component" reachable from "../"
set(BASE_PATH_TO_INCLUDE "../components/bemesh/")
list(APPEND INCLUDE_LIST )


#For each required component we build the string "../component/include" that will be added to
#the INCLUDE_LIST variable
foreach(item ${REQUIRED_COMPS})
    set(tmp "${BASE_PATH_TO_INCLUDE}${item}/include")
    list(APPEND INCLUDE_LIST ${tmp})
endforeach()



#Finally we register the component using the values stored in the variables above
idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS ${INCLUDE_LIST} 
                        REQUIRES nvs_flash bt kernel mid-tier callbacks common firmware)

