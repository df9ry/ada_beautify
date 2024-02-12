cmake_minimum_required(VERSION 3.10)

project(ada_beautify LANGUAGES C CXX VERSION 0.1.0
        DESCRIPTION "Tool to make adapuff files compileable")

cmake_policy(SET CMP0115 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    document.cpp
    formatter.cpp
    getopt.c
    main.cpp
    symbol.cpp
)

set(HEADERS
    document.hpp
    formatter.hpp
    getopt.h
    scanner.hpp
    scope.hpp
    symbol.hpp
    utils.hpp
    version.hpp.in
)

add_executable(ada_beautify ${SOURCES} ${HEADERS})
target_compile_features(ada_beautify PUBLIC cxx_std_20)
target_include_directories(ada_beautify PUBLIC
    BEFORE "${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}/include/"
    BEFORE "${PROJECT_BINARY_DIR}")
target_link_directories(ada_beautify PUBLIC
    BEFORE "${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}/lib/")
configure_file(version.hpp.in version.hpp)

include(GNUInstallDirs)
install(TARGETS ada_beautify
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
