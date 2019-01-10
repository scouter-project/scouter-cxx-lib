
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

cmake_minimum_required (VERSION 2.8)
project(scoutercxx )
#SET(CMAKE_CXX_FLAGS "-std=c++0x"),
SET(CMAKE_CXX_FLAGS "-std=gnu++98")
# enable display command while building.
SET(CMAKE_VERBOSE_MAKEFILE on)
#message("current dir: ${CMAKE_CURRENT_SOURCE_DIR}")
set(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build) 
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
set(CMAKE_BUILD_TYPE Debug)
# source file
file(GLOB_RECURSE SRCS src/*.cc)

#make a shared library
add_library(scoutercxx SHARED ${SRCS})

# include file
target_include_directories(scoutercxx PUBLIC ${INC_DIR})

#lower cmake version. use #include_directories
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#install
install(TARGETS scoutercxx
        LIBRARY DESTINATION ${TARGET_DIR})

#install(TARGETS scoutercxx
#    LIBRARY DESTINATION ${TARGET_DIR}
#    PUBLIC_HEADER DESTINATION ${TARGET_DIR}/include)
   
install(DIRECTORY ${INC_DIR} DESTINATION ${TARGET_DIR}/include)



