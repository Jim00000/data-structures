# SPDX-FileCopyrightText: 2026 Jim00000
#
# SPDX-License-Identifier: BSD-3-Clause

# Required packages
find_package (GTest 1.17 REQUIRED)

# Declare test targets
add_executable (unittest)

# Compile Options
target_compile_features (unittest PUBLIC cxx_std_23)
target_compile_options (unittest PRIVATE
    -Wall -Wextra -Wshadow
    $<$<CONFIG:Debug>:-g3 -O0 -fno-omit-frame-pointer>
    $<$<BOOL:${ENABLE_COVERAGE}>:${COVERAGE_FLAGS}>
    $<$<BOOL:${ENABLE_SANITIZE}>:-fsanitize=address,undefined,leak>
)

# Definitions
target_compile_definitions (unittest PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)

# Link Options
target_link_options (unittest PRIVATE
    $<$<BOOL:${ENABLE_COVERAGE}>:${COVERAGE_FLAGS}>
    $<$<BOOL:${ENABLE_SANITIZE}>:-fsanitize=address,undefined,leak>
)

# Sources
target_sources (unittest PRIVATE main_test.cpp)

# subsources
add_subdirectory (lists)

# Link libraries
target_link_libraries (unittest PRIVATE ds GTest::gtest GTest::gtest_main)

# Discover testcases
gtest_discover_tests (unittest)

# Post-build trigger
if (ENABLE_COVERAGE)
    message (STATUS "Attach post-build code coverage trigger into unittest")
    add_custom_command (TARGET unittest POST_BUILD
        COMMAND ${CMAKE_COMMAND} --build --preset codecov_build --target coverage
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Trigger Code Coverage Task..."
    )
endif ()