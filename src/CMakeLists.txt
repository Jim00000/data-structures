# SPDX-FileCopyrightText: 2026 Jim00000
#
# SPDX-License-Identifier: BSD-3-Clause

# Declare targets
add_library (ds STATIC)

# Setup include path
target_include_directories (ds PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Setup compile options
target_compile_options (ds PRIVATE
    -Wall -Wextra -Wshadow
    $<$<CONFIG:Debug>:-g3 -O0 -fno-omit-frame-pointer>
    $<$<CONFIG:Release>:-O3 -fstack-protector-strong -s -D__FORTIFY_SOURCE=2 -Wl,-O1>
    $<$<BOOL:${ENABLE_COVERAGE}>:${COVERAGE_FLAGS}>
    $<$<BOOL:${ENABLE_SANITIZE}>:-fsanitize=address,undefined,leak>
)

# Compile Options
target_compile_features (ds PUBLIC c_std_23)
set_property (TARGET ds PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
set_property (TARGET ds PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# Definitions
target_compile_definitions (ds PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)

# Setup link options
target_link_options (ds PRIVATE
    $<$<BOOL:${ENABLE_COVERAGE}>:${COVERAGE_FLAGS}>
    $<$<BOOL:${ENABLE_SANITIZE}>:-fsanitize=address,undefined,leak>
)

# Source 
target_sources (ds PRIVATE
    alloc.c
)

# Source modules
add_subdirectory (lists)

# Build docs if enabled
if (BUILD_DOCS)
    message (STATUS "Attach post-build doxygen generation")
    add_custom_command (TARGET ds POST_BUILD
        COMMAND ${CMAKE_COMMAND} --build --preset release_build --target doxygen
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Trigger Doxygen Documentations Generation Task..."
    )
endif ()
