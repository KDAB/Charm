# Copyright (C) 2015-2017 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#
# Try to find XCB library and include path.
# Once done this will define
#
# XCB_FOUND
# XCB_INCLUDE_PATH
# XCB_LIBRARIES
# XCB_SCREENSAVER_LIBRARIES

if(UNIX AND NOT APPLE)
    find_path(XCB_INCLUDE_PATH xcb/xcb.h
        /usr/include
        DOC "The directory where xcb/xcb.h resides"
    )

    find_library(XCB_LIBRARIES
        NAMES xcb
        PATHS
        /usr/lib
        DOC "The xcb library"
    )

    find_library(XCB_SCREENSAVER_LIBRARIES
        NAMES xcb-screensaver
        PATHS
        /usr/lib
        DOC "The xcb-screensaver library"
    )
endif()

if(XCB_INCLUDE_PATH AND XCB_LIBRARIES AND XCB_SCREENSAVER_LIBRARIES)
    set(XCB_FOUND 1)
else()
    set(XCB_FOUND 0)
endif()

mark_as_advanced(XCB_INCLUDE_PATH XCB_LIBRARIES XCB_SCREENSAVER_LIBRARIES)
