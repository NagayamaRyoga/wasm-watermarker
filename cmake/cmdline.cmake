ExternalProject_Add(cmdline
    GIT_REPOSITORY  "https://github.com/tanakh/cmdline.git"
    GIT_TAG         "master"
    PREFIX          "${CMAKE_CURRENT_BINARY_DIR}/cmdline"
    SOURCE_DIR      "${CMAKE_CURRENT_BINARY_DIR}/cmdline/src"
    BINARY_DIR      "${CMAKE_CURRENT_BINARY_DIR}/cmdline/build"
    STAMP_DIR       "${CMAKE_CURRENT_BINARY_DIR}/cmdline/stamp"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND   ""
    UPDATE_COMMAND  ""
    INSTALL_COMMAND ""
    TEST_COMMAND    ""
)

ExternalProject_Get_Property(cmdline source_dir)
ExternalProject_Get_Property(cmdline binary_dir)

add_library(cmdline::cmdline INTERFACE IMPORTED)

make_directory("${source_dir}") # To suppress non-exist directory warnings

target_include_directories(cmdline::cmdline INTERFACE
    "${source_dir}"
)

add_dependencies(cmdline::cmdline cmdline)
