# SPDX-FileCopyrightText: 2026 Jim00000
#
# SPDX-License-Identifier: BSD-3-Clause
include_guard (DIRECTORY)

find_package (Doxygen 1.16.1 REQUIRED)

if (DOXYGEN_FOUND)
    set (DOXYGEN_GENERATE_HTML YES)
    set (DOXYGEN_GENERATE_LATEX NO)
    set (DOXYGEN_PROJECT_NAME "Singly Linked List Project")
    set (DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs")
    set (DOXYGEN_HAVE_DOT YES)
    set (DOXYGEN_EXTRACT_ALL YES)

    doxygen_add_docs (doxygen
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        COMMENT "Generating API documentation with Doxygen..."
    )

    add_custom_command (TARGET doxygen POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Documentation generated: ${CMAKE_BINARY_DIR}/docs/html/index.html"
    )
else ()
    message (SEND_ERROR "doxygen not found")
endif ()