if(NOT DEFINED ESP32_SIMULATION)
  list(APPEND REQUIRED_COMPS "kernel" "mid-tier" "common" "master" "slave")


  #In this variable we keep track of source files needed by the component.
  list(APPEND SOURCES "callbacks.cpp")

  #All header files will be located inside the "include" directory of the component "component" reachable from "../"
  set(BASE_PATH_TO_INCLUDE "../")
  list(APPEND INCLUDE_LIST )


  #For each required component we build the string "../component/include" that will be added to
  #the INCLUDE_LIST variable
  foreach(item ${REQUIRED_COMPS})
    set(tmp "${BASE_PATH_TO_INCLUDE}${item}/include")
    list(APPEND INCLUDE_LIST ${tmp})
  endforeach()

  #We append to the INCLUDE_LIST the include directory of this component
  list(APPEND INCLUDE_LIST "include")


  #Finally we register the component using the values stored in the variables above
  idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS ${INCLUDE_LIST} 
    REQUIRES mid-tier kernel common master slave)
else()
  # Builds the libraries
  project(firmware)
  add_library(Firmware firmware.cpp)
  target_include_directories(Firmware PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
  target_link_libraries(Firmware Master Slave Kernel EspLog Callbacks Common Router)

  
endif(NOT DEFINED ESP32_SIMULATION)
