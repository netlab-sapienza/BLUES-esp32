
if(NOT DEFINED ESP32_SIMULATION)

  list(APPEND REQUIRED_COMPS)


  #In this variable we keep track of source files needed by the component.
  list(APPEND SOURCES "bemesh_messages_v2.cpp" "message_handler_v2.cpp" "routing.cpp" "rtable.cpp")


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
    INCLUDE_DIRS ${INCLUDE_LIST})

else()
  # Builds the libraries
  project(mid-tier)
  #add_library(RTable rtable.cpp)
  #target_include_directories(RTable PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

  #add_library(BemeshMessages bemesh_messages_v2.cpp)
  #target_include_directories(BemeshMessages PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

  add_library(Routing rtable.cpp routing.cpp bemesh_messages_v2.cpp message_handler_v2.cpp)
  target_include_directories(Routing PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

  #add_library(MessageHandler message_handler_v2.cpp)
  #target_include_directories(MessageHandler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

  #target_link_libraries(Routing RTable BemeshMessages)
  
  
		      
endif(NOT DEFINED ESP32_SIMULATION)


