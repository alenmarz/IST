cmake_minimum_required(VERSION 3.17)
project(IST)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_executable(IST
        Element.hpp
        Element.h
        main.cpp
        Node.hpp
        Node.h
        README.md
        Tree.hpp
        Tree.h
        TreapNode.hpp
        TreapNode.h
        Treap.hpp
        Treap.h
        Action.h
        Action.hpp)

target_include_directories(IST PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/sptl/include>
        $<INSTALL_INTERFACE:lib/sptl/include>
        )

target_include_directories(IST PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/chunkedseq/include>
        $<INSTALL_INTERFACE:lib/chunkedseq/include>
        )

target_include_directories(IST PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/cmdline/include>
        $<INSTALL_INTERFACE:lib/cmdline/include>
        )