cmake_minimum_required(VERSION 3.5)

set(lib_path ${COMPONENT_DIR}/lib)
set(include_dirs lib include)

idf_component_register(
    INCLUDE_DIRS "${include_dirs}"
    REQUIRES "pthread"
)

ExternalProject_Add(protobuf_build
    PREFIX ${COMPONENT_DIR}
    SOURCE_DIR ${COMPONENT_DIR}/protobuf/src
    BUILD_BYPRODUCTS ${COMPONENT_DIR}/lib/libprotobuf.a
    CONFIGURE_COMMAND ""
    BINARY_DIR ${COMPONENT_DIR}
    BUILD_COMMAND make libprotobuf.a
    INSTALL_COMMAND ""
)

# Add libprotobuf.a to the build process
add_library(protobuf STATIC IMPORTED GLOBAL)
add_dependencies(protobuf protobuf_build)

target_link_libraries(${COMPONENT_LIB} INTERFACE "${lib_path}/libprotobuf.a")
set_target_properties(protobuf PROPERTIES IMPORTED_LOCATION
     ${COMPONENT_DIR}/lib/libprotobuf.a)

 set_target_properties(protobuf PROPERTIES COMPONENT_SRCDIRS
     ${COMPONENT_DIR}/protobuf/src)

set_target_properties(protobuf PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
     ${COMPONENT_DIR}/include)

 set_target_properties(protobuf PROPERTIES CXX_COMPILE_OPTIONS
     "-std=c++14")

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
     "${COMPONENT_DIR}/lib/libprotobuf.a")
