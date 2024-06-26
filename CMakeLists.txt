# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(counter)

FILE(GLOB app_sources src/*.c src/test_threads/*.c)
target_sources(app PRIVATE ${app_sources})


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -g0")


if(DEFINED DEBUG_PRINTF)
  zephyr_compile_definitions(DEBUG_PRINTF=${DEBUG_PRINTF})
endif()
if(DEFINED SAME_PRIO)
  zephyr_compile_definitions(SAME_PRIO)
endif()
if(DEFINED FORKS)
  zephyr_compile_definitions(FORKS=${FORKS})
endif()

target_include_directories(app PRIVATE ${ZEPHYR_BASE}/include/zephyr/portability inc)



