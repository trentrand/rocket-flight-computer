cmake_minimum_required(VERSION 3.5)

set(lib_path ${COMPONENT_DIR}/c-capnproto/lib)

idf_component_register(
    INCLUDE_DIRS "${include_dirs}"
)

ExternalProject_Add(c-capnproto_build
    PREFIX ${COMPONENT_DIR}
    SOURCE_DIR ${COMPONENT_DIR}/c-capnproto
    BUILD_BYPRODUCTS ${COMPONENT_DIR}/build
    CONFIGURE_COMMAND ""
    BINARY_DIR ${COMPONENT_DIR}
    BUILD_COMMAND make libcapnp_c.a
    INSTALL_COMMAND ""
)
