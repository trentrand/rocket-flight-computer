cmake_minimum_required(VERSION 3.5)

set(lib_path ${COMPONENT_DIR}/c-capnproto/lib)

idf_component_register(
    INCLUDE_DIRS "./c-capnproto/lib"
)

ExternalProject_Add(c-capnproto_build
    PREFIX ${COMPONENT_DIR}
    SOURCE_DIR ${COMPONENT_DIR}/c-capnproto
    BUILD_BYPRODUCTS ${COMPONENT_DIR}/c-capnproto/.libs
    CONFIGURE_COMMAND ""
    BINARY_DIR ${COMPONENT_DIR}
    BUILD_COMMAND make build
    INSTALL_COMMAND ""
)
