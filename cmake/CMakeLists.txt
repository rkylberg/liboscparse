cmake_minimum_required(VERSION 3.0)

option(WITH_STATIC "Enable building static library." ON)

if (WITH_STATIC)
  message(STATUS "If you are using the static library build, please keep in mind (and inform yourself of the implications) that liblo is licensed with LGPL v2.1+.")
endif()

include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckLibraryExists)
include(TestBigEndian)
include(GNUInstallDirs)

set(PROJECT liblop)
set(PACKAGE_NAME "${PROJECT}")
project(${PROJECT} LANGUAGES C)

# create compile cmds db useful for creating SourceTrail project:
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_VERSION VERSION_LESS 3.1.0)
    set_property(TARGET ${LIBRARY} PROPERTY C_STANDARD 11)
endif()

set(CMAKE_VERBOSE_MAKEFILE OFF)

if (MSVC)
  add_definitions(
    /D_CRT_SECURE_NO_WARNINGS
    /D_CRT_SECURE_NO_DEPRECATE
    /D_CRT_NONSTDC_NO_DEPRECATE
    /D_WINSOCK_DEPRECATED_NO_WARNINGS)

  # On Windows we do not wish the default behaviour of removing "lib"
  # from "liblop.dll".
  if ("${CMAKE_SHARED_LIBRARY_PREFIX}" STREQUAL "")
    set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
  endif()
  if ("${CMAKE_STATIC_LIBRARY_PREFIX}" STREQUAL "")
    set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
  endif()

  # For Visual Studio, ensure import lib can be distinguished from
  # static lib
  if ("${CMAKE_IMPORT_LIBRARY_PREFIX}" STREQUAL "")
    set(CMAKE_IMPORT_LIBRARY_PREFIX "lib")
  endif()
  if ("${CMAKE_IMPORT_LIBRARY_SUFFIX}" STREQUAL ".lib")
    set(CMAKE_STATIC_LIBRARY_SUFFIX "_static.lib")
  endif()
endif()

set(LIBRARY_SHARED lop_shared)
if (WITH_STATIC)
  set(LIBRARY_STATIC lop_static)
endif()

set(LIBRARY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../blob.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../message.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../method.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../pattern_match.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../timetag.c
    )

set(LIBRARY_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/../lop_types_internal.h
    ${CMAKE_CURRENT_BINARY_DIR}/../lop_internal.h
    ${CMAKE_CURRENT_BINARY_DIR}/../lop/lop_endian.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../lop/lop_errors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../lop/lop_lowlevel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../lop/lop_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../lop/lop_osc_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../lop/lop_throw.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../lop/lop_types.h
    )

# check_symbol_exists(poll poll.h HAVE_POLL)
# check_symbol_exists(select sys/select.h HAVE_SELECT)
# if(NOT HAVE_POLL AND NOT HAVE_SELECT)
#   if(CMAKE_SYSTEM_NAME MATCHES "Windows")
#     set(HAVE_SELECT 1)
#     message(STATUS "Windows configuration: Assuming select exists.")
#   else()
#     message(FATAL_ERROR "Neither select nor poll was found.")
#   endif()
# endif()
# check_symbol_exists(getifaddrs "sys/types.h;ifaddrs.h" HAVE_GETIFADDRS)
# check_symbol_exists(inet_pton "arpa/inet.h" HAVE_INET_PTON)

set(BUILD_LANGUAGE C CACHE STRING "Build language (C or CXX)")
mark_as_advanced(BUILD_LANGUAGE)
set_source_files_properties(
    ${LIBRARY_SOURCES}
    PROPERTIES LANGUAGE ${BUILD_LANGUAGE})

# Library
add_library(${LIBRARY_SHARED} SHARED ${LIBRARY_SOURCES})
if (WITH_STATIC)
  add_library(${LIBRARY_STATIC} STATIC ${LIBRARY_SOURCES})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(${LIBRARY_SHARED} "user32" "wsock32" "ws2_32" "iphlpapi")

    set_target_properties(${LIBRARY_SHARED} PROPERTIES
        COMPILE_DEFINITIONS "LIBLOP_DLL")
    if (WITH_STATIC)
      set_target_properties(${LIBRARY_STATIC} PROPERTIES
          COMPILE_DEFINITIONS "LIBLOP_LIB")
    endif()

    set(DLLNAME "")
    set(DEFTHREADS ";;")
else()
  target_link_libraries(${LIBRARY_SHARED} "m")
  if (WITH_STATIC)
    target_link_libraries(${LIBRARY_STATIC} "m")
  endif()
endif()

set(LOP_INCLUDE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(${LIBRARY_SHARED} PUBLIC ${LOP_INCLUDE_DIRS})
if (WITH_STATIC)
  target_include_directories(${LIBRARY_STATIC} PUBLIC ${LOP_INCLUDE_DIRS})
endif()

# parse version info from configure.ac
# file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../configure.ac"
#     CONFIGURE_AC_LINE REGEX "AC_INIT\\(\\[liblo\\].*\\)")
# string(REGEX MATCHALL "[0-9]+" CONFIGURE_AC_LIST ${CONFIGURE_AC_LINE})
# list(GET CONFIGURE_AC_LIST 0 CONFIGURE_AC_MAJOR)
# list(GET CONFIGURE_AC_LIST 1 CONFIGURE_AC_MINOR)
# set(PACKAGE_VERSION "${CONFIGURE_AC_MAJOR}.${CONFIGURE_AC_MINOR}")

# file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../configure.ac"
#     CONFIGURE_AC_LINE REGEX "m4_define\\(\\[lt_current\\], [0-9]+\\)")
# string(REGEX MATCHALL "[0-9]+" CONFIGURE_AC_LIST ${CONFIGURE_AC_LINE})
# list(GET CONFIGURE_AC_LIST 1 CONFIGURE_AC_CURRENT)
# file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../configure.ac"
#     CONFIGURE_AC_LINE REGEX "m4_define\\(\\[lt_revision\\], [0-9]+\\)")
# string(REGEX MATCHALL "[0-9]+" CONFIGURE_AC_LIST ${CONFIGURE_AC_LINE})
# list(GET CONFIGURE_AC_LIST 1 CONFIGURE_AC_REVISION)
# file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../configure.ac"
#     CONFIGURE_AC_LINE REGEX "m4_define\\(\\[lt_age\\], [0-9]+\\)")
# string(REGEX MATCHALL "[0-9]+" CONFIGURE_AC_LIST ${CONFIGURE_AC_LINE})
# list(GET CONFIGURE_AC_LIST 1 CONFIGURE_AC_AGE)
# set(LO_SO_VERSION
#     "{${CONFIGURE_AC_CURRENT}, ${CONFIGURE_AC_REVISION}, ${CONFIGURE_AC_AGE}}")

# math(EXPR CURRENT_MINUS_AGE "${CONFIGURE_AC_CURRENT} - ${CONFIGURE_AC_AGE}")
# set_target_properties(${LIBRARY_SHARED} PROPERTIES
#     VERSION   "${CURRENT_MINUS_AGE}.${CONFIGURE_AC_AGE}.${CONFIGURE_AC_REVISION}"
#     SOVERSION "${CURRENT_MINUS_AGE}"
#     OUTPUT_NAME "lo"
#     )
if (WITH_STATIC)
  set_target_properties(${LIBRARY_STATIC} PROPERTIES
      OUTPUT_NAME "lop"
      )
endif()

# if(NOT DEFINED PRINTF_LL)
#     message(STATUS "Check how to print long long int")
#     try_compile(PRINTF_LL_AVAILABLE ${CMAKE_CURRENT_BINARY_DIR}
#         SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test-printf-ll.c")
#     if(PRINTF_LL_AVAILABLE)
#         set(PRINTF_LL "ll" CACHE INTERNAL "")
#     else()
#         try_compile(PRINTF_LL_AVAILABLE ${CMAKE_CURRENT_BINARY_DIR}
#             SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test-printf-I64.c")
#         if(PRINTF_LL_AVAILABLE)
#             set(PRINTF_LL "I64" CACHE INTERNAL "")
#         else()
#             message(FATAL_ERROR "printf doesn't support long long int")
#         endif()
#     endif()
# endif()

# if(MSVC)
#     set(VERSION "${CONFIGURE_AC_CURRENT}.${CONFIGURE_AC_REVISION}")
#     configure_file(
#         ${CMAKE_CURRENT_SOURCE_DIR}/config-msvc.h.in
#         ${CMAKE_CURRENT_BINARY_DIR}/config.h)
#     configure_file(
#         ${CMAKE_CURRENT_SOURCE_DIR}/../src/liblo.def.in
#         ${CMAKE_CURRENT_BINARY_DIR}/src/liblo.def)
#     if (WITH_STATIC)
#       set_target_properties(${LIBRARY_STATIC} PROPERTIES
#           LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_BINARY_DIR}/src/liblo.def\"")
#     endif()
#     set_target_properties(${LIBRARY_SHARED} PROPERTIES
#         LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_BINARY_DIR}/src/liblo.def\"")
# else()
#     configure_file(
#         ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
#         ${CMAKE_CURRENT_BINARY_DIR}/config.h)
# endif()

# configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/../lo/lo.h.in
#     ${CMAKE_CURRENT_BINARY_DIR}/lo/lo.h)
# test_big_endian(LO_BIGENDIAN)
# configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/../lo/lo_endian.h.in
#     ${CMAKE_CURRENT_BINARY_DIR}/lo/lo_endian.h)
# configure_file(
#     ${PROJECT}Config.cmake.in
#     ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}Config.cmake)

# set(prefix ${CMAKE_INSTALL_PREFIX})
# set(exec_prefix "\${prefix}")
# set(libdir "\${exec_prefix}/lib")
# set(includedir "\${prefix}/include")
# set(LIBPTHREAD ${CMAKE_THREAD_LIBS_INIT})
# configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT}.pc.in
#     ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}.pc
#     @ONLY)
# add_definitions(-DHAVE_CONFIG_H)

# install(
#     TARGETS ${LIBRARY_STATIC} ${LIBRARY_SHARED}
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
#     RUNTIME DESTINATION bin)
# install(
#     FILES ${LIBRARY_HEADERS}
#     DESTINATION include/lo)
# install(
#     FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}Config.cmake
#     DESTINATION lib/cmake/${PROJECT})
# install(
#     FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}.pc
#     DESTINATION lib/pkgconfig)
