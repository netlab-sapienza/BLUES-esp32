
list(APPEND REQUIRED_COMPS "kernel")


#In this variable we keep track of source files needed by the component.
list(APPEND SOURCES "common.cpp")


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
                        REQUIRES nvs_flash bt services kernel)

