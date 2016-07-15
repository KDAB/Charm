#.rst:
# ECMQtDeclareLoggingCategory
# ---------------------------
#
# Generate declarations for logging categories in Qt5.
#
# ::
#
#   ecm_qt_declare_logging_category(<sources_var>
#                                   HEADER <filename>
#                                   IDENTIFIER <identifier>
#                                   CATEGORY_NAME <category_name>
#                                   [DEFAULT_SEVERITY
#                                        <Debug|Info|Warning|
#                                         Critical|Fatal>])
#
# A header file, ``<filename>``, will be generated along with a corresponding
# source file, which will be added to ``<sources_var>``. These will provide a
# QLoggingCategory category that can be referred to from C++ code using
# ``<identifier>``, and from the logging configuration using
# ``<category_name>``.
#
# If ``<filename>`` is not absolute, it will be taken relative to the current
# binary directory.
#
# If the code is compiled against Qt 5.4 or later, by default it will only log
# output that is at least the severity specified by ``DEFAULT_SEVERITY``, or
# "Warning" level if ``DEFAULT_SEVERITY`` is not given. Note that, due to a
# bug in Qt 5.5, "Info" may be treated as more severe than "Fatal".
#
# ``<identifier>`` may include namespaces (eg: ``foo::bar::IDENT``).
#
# Since 5.14.0.

#=============================================================================
# Copyright 2015 Alex Merry <alex.merry@kde.org>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file COPYING-CMAKE-SCRIPTS for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of extra-cmake-modules, substitute the full
#  License text for the above reference.)

include(CMakeParseArguments)

set(_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP "${CMAKE_CURRENT_LIST_DIR}/ECMQtDeclareLoggingCategory.cpp.in")
set(_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_H   "${CMAKE_CURRENT_LIST_DIR}/ECMQtDeclareLoggingCategory.h.in")

function(ecm_qt_declare_logging_category sources_var)
    set(options)
    set(oneValueArgs HEADER IDENTIFIER CATEGORY_NAME DEFAULT_SEVERITY)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unexpected arguments to ecm_qt_declare_logging_category: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT ARG_HEADER)
        message(FATAL_ERROR "Missing HEADER argument for ecm_qt_declare_logging_category")
    endif()
    if(NOT ARG_IDENTIFIER)
        message(FATAL_ERROR "Missing IDENTIFIER argument for ecm_qt_declare_logging_category")
    endif()
    if(NOT ARG_CATEGORY_NAME)
        message(FATAL_ERROR "Missing CATEGORY_NAME argument for ecm_qt_declare_logging_category")
    endif()
    if(NOT ARG_DEFAULT_SEVERITY)
        set(ARG_DEFAULT_SEVERITY Warning)
    else()
        set(acceptible_severities Debug Info Warning Critical Fatal)
        list(FIND acceptible_severities "${ARG_DEFAULT_SEVERITY}" pos)
        if (pos EQUAL -1)
            message(FATAL_ERROR "Unknown DEFAULT_SEVERITY ${pos}")
        endif()
    endif()

    if (NOT IS_ABSOLUTE "${ARG_HEADER}")
        set(ARG_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${ARG_HEADER}")
    endif()

    string(REPLACE "::" ";" namespaces "${ARG_IDENTIFIER}")
    list(LENGTH namespaces len)
    math(EXPR last_pos "${len} - 1")
    list(GET namespaces ${last_pos} IDENTIFIER)
    list(REMOVE_AT namespaces ${last_pos})

    set(OPEN_NAMESPACES)
    set(CLOSE_NAMESPACES)
    foreach(ns ${namespaces})
        set(OPEN_NAMESPACES "${OPEN_NAMESPACES} namespace ${ns} {")
        set(CLOSE_NAMESPACES "} ${CLOSE_NAMESPACES}")
    endforeach()

    string(FIND "${ARG_HEADER}" "." pos REVERSE)
    if (pos EQUAL -1)
        set(cpp_filename "${ARG_HEADER}.cpp")
    else()
        string(SUBSTRING "${ARG_HEADER}" 0 ${pos} cpp_filename)
        set(cpp_filename "${cpp_filename}.cpp")
    endif()

    get_filename_component(HEADER_NAME "${ARG_HEADER}" NAME)

    string(REPLACE "::" "_" GUARD_NAME "${ARG_IDENTIFIER}_H")
    string(TOUPPER "${GUARD_NAME}" GUARD_NAME)

    if (NOT _ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP)
       message(FATAL_ERROR "You must include(ECMQtDeclareLoggingCategory) before using ecm_qt_declare_logging_category")
    endif()

    configure_file("${_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP}" "${cpp_filename}")
    configure_file("${_ECM_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_H}" "${ARG_HEADER}")

    set(sources "${${sources_var}}")
    list(APPEND sources "${cpp_filename}")
    set(${sources_var} "${sources}" PARENT_SCOPE)
endfunction()

