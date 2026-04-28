# SPDX-FileCopyrightText: 2026 Jim00000
#
# SPDX-License-Identifier: BSD-3-Clause
include_guard (DIRECTORY)

find_program (CPPCHECK_EXE cppcheck REQUIRED)

if (NOT CPPCHECK_EXE-NOTFOUND)
    set (CMAKE_C_CPPCHECK 
        ${CPPCHECK_EXE} 
        "--suppress=missingIncludeSystem"
        "--suppress=unusedFunction"
        "--enable=all" 
        "--inline-suppr" 
        "--inconclusive" 
        "--quiet"
    )
else ()
    message (SEND_ERROR "cppcheck not found")
endif ()