#.rst:
# KDEFrameworkCompilerSettings
# ----------------------------
#
# Set stricter compile and link flags for KDE Frameworks modules.
#
# The KDECompilerSettings module is included and, in addition, various
# defines that affect the Qt libraries are set to enforce certain
# conventions.
#
# For example, constructions like QString("foo") are prohibited, instead
# forcing the use of QLatin1String or QStringLiteral, and some
# Qt-defined keywords like signals and slots will not be defined.
#
# NB: it is recommended to include this module with the NO_POLICY_SCOPE
# flag, otherwise you may get spurious warnings with some versions of CMake.
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2013      Albert Astals Cid <aacid@kde.org>
# Copyright 2007      Matthias Kretz <kretz@kde.org>
# Copyright 2006-2007 Laurent Montel <montel@kde.org>
# Copyright 2006-2013 Alex Neundorf <neundorf@kde.org>
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

include(KDECompilerSettings NO_POLICY_SCOPE)

add_definitions(-DQT_NO_CAST_TO_ASCII
                -DQT_NO_CAST_FROM_ASCII
                -DQT_NO_URL_CAST_FROM_STRING
                -DQT_NO_CAST_FROM_BYTEARRAY
                -DQT_NO_SIGNALS_SLOTS_KEYWORDS
                -DQT_USE_FAST_OPERATOR_PLUS
                -DQT_USE_QSTRINGBUILDER
               )

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DQT_STRICT_ITERATORS)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
   # -Wgnu-zero-variadic-macro-arguments is triggered by every qCDebug() call and therefore results
   # in a lot of noise. This warning is only notifying us that clang is emulating the GCC behaviour
   # instead of the exact standard wording so we can safely ignore it
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wno-gnu-zero-variadic-macro-arguments")
endif()
