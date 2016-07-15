#.rst:
# ECMCoverageOption
# --------------------
#
# Allow users to easily enable GCov code coverage support.
#
# Code coverage allows you to check how much of your codebase is covered by
# your tests. This module makes it easy to build with support for
# `GCov <https://gcc.gnu.org/onlinedocs/gcc/Gcov.html>`_.
#
# When this module is included, a ``BUILD_COVERAGE`` option is added (default
# OFF). Turning this option on enables GCC's coverage instrumentation, and
# links against ``libgcov``.
#
# Note that this will probably break the build if you are not using GCC.
#
# Since 1.3.0.

#=============================================================================
# Copyright 2014 Aleix Pol Gonzalez <aleixpol@kde.org>
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

option(BUILD_COVERAGE "Build the project with gcov support" OFF)

if(BUILD_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov")
endif()
