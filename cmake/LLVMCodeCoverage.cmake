# SPDX-FileCopyrightText: 2026 Jim00000
#
# SPDX-License-Identifier: BSD-3-Clause
include_guard (DIRECTORY)

set (COVERAGE_ROOTDIR coverage_report)
set (COVERAGE_LLVM_PROFILE unittest.profraw)
set (COVERAGE_LLVM_PROFDATA unittest.profdata)

find_program (LLVM_PROFDATA_PATH llvm-profdata)
find_program (LLVM_COV_PATH llvm-cov)

add_custom_target (coverage
    COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${COVERAGE_LLVM_PROFILE} $<TARGET_FILE:unittest>
    COMMAND ${LLVM_PROFDATA_PATH} merge -sparse ${COVERAGE_LLVM_PROFILE} -o ${COVERAGE_LLVM_PROFDATA}
    COMMAND ${LLVM_COV_PATH} show $<TARGET_FILE:unittest> 
        -instr-profile=${COVERAGE_LLVM_PROFDATA} 
        -format=html 
        -output-dir=${COVERAGE_ROOTDIR} 
        -ignore-filename-regex="tests/" 
        -show-line-counts-or-regions 
        -show-instantiations
    DEPENDS unittest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    USES_TERMINAL
    COMMENT "Generating Code Coverage report..."
)