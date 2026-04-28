# SPDX-FileCopyrightText: 2026 Jim00000
#
# SPDX-License-Identifier: BSD-3-Clause
include_guard (DIRECTORY)

set (COVERAGE_ROOTDIR coverage_report)
find_program (LCOV_PATH lcov)
find_program (GENHTML_PATH genhtml)
add_custom_target (coverage
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMAND ${LCOV_PATH} --capture --rc geninfo_unexecuted_blocks=1 --keep-going --ignore-errors mismatch,mismatch --directory . --output-file lcov.info --quiet
    COMMAND ${LCOV_PATH} --remove lcov.info '/usr/*' '*/tests/*' --output-file lcov.info --quiet
    COMMAND ${GENHTML_PATH} lcov.info --output-directory ${COVERAGE_ROOTDIR}
    COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated: ${CMAKE_BINARY_DIR}/${COVERAGE_ROOTDIR}/index.html"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating Code Coverage report..."
)