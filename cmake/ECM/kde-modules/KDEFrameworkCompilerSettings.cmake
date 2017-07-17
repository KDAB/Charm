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
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0.0")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-override" )
   endif()
endif()
