# SPDX-FileCopyrightText: 2026 Jim00000
#
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required (VERSION 4.3)

project (data-structures 
    VERSION 0.1.0.0
    LANGUAGES C CXX
)

option (BUILD_DOCS "Build doxygen documentation" OFF)
option (ENABLE_CLANG_TIDY "Enable clang-tidy check" OFF)
option (ENABLE_COVERAGE "Enable code coverage" OFF)
option (ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option (ENABLE_SANITIZE "Enable sanitize" OFF)
option (ENABLE_TEST "Enable testing" OFF)
option (ENABLE_CLANG "Enable Clang toolchain" OFF)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build Type
message (STATUS "Build Type : ${CMAKE_BUILD_TYPE}")

if (ENABLE_CLANG)
    message (STATUS "Clang/LLVM Toolchain is USED")
else ()
    message (STATUS "GNU Toolchain is USED")
endif ()

# (Optional) Doxygen
if (BUILD_DOCS)
    message (STATUS "Doxygen is ENABLED")
    include (cmake/Docs.cmake)
else ()
    message (STATUS "Doxygen is DISABLED")
endif ()

# (Optional) Clang-Tidy
if (ENABLE_CLANG_TIDY)
    message (STATUS "Clang-Tidy is ENABLED")
    include (cmake/ClangTidy.cmake)
else ()
    message (STATUS "Clang-Tidy is DISABLED")
endif ()

# (Optional) Code Coverage
if (ENABLE_COVERAGE)
    message (STATUS "Code Coverage is ENABLED")
    if (ENABLE_CLANG)
        include (cmake/LLVMCodeCoverage.cmake)
        set (COVERAGE_FLAGS -fprofile-instr-generate -fcoverage-mapping)
    else ()
        include (cmake/GNUCodeCoverage.cmake)
        set (COVERAGE_FLAGS --coverage)
    endif ()
else ()
    message (STATUS "Code Coverage is DISABLED")
endif ()

# (Optional) CppCheck
if (ENABLE_CPPCHECK)
    message (STATUS "CppCheck is ENABLED")
    include (cmake/Cppcheck.cmake)
else ()
    message (STATUS "CppCheck is DISABLED")
endif ()

# (Optional) Sanitize
if (ENABLE_SANITIZE)
    message (STATUS "Sanitize is ENABLED")
else ()
    message (STATUS "Sanitize is DISABLED")
endif ()

# Source files
add_subdirectory (src)

# (Optional) Test
if (ENABLE_TEST)
    message (STATUS "Test is ENABLED")
    enable_testing ()
    include (CTest)
    add_subdirectory (tests)
else ()
    message (STATUS "Test is DISABLED")
endif ()
