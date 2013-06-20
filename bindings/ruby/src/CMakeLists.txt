cmake_minimum_required(VERSION 2.6)
PROJECT(rbind_framehelper CXX)

include(FindPkgConfig)
pkg_check_modules(BASE-TYPES REQUIRED base-types)
add_definitions(${BASE-TYPES_CFLAGS})
include_directories(${BASE-TYPES_INCLUDE_DIRS})
link_directories(${BASE-TYPES_LIBRARY_DIRS})

pkg_check_modules(RBIND_OPENCV REQUIRED rbind_opencv)
add_definitions(${RBIND_OPENCV_CFLAGS})
include_directories(${RBIND_OPENCV_INCLUDE_DIRS})
link_directories(${RBIND_OPENCV_LIBRARY_DIRS})

SET(RBIND_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/types.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/operations.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/conversions.cc")

add_custom_command(OUTPUT ${RBIND_SRC}
   DEPENDS frame_helper
   COMMAND ruby "${CMAKE_CURRENT_SOURCE_DIR}/../generate.rb")

ADD_LIBRARY(rbind_framehelper SHARED ${RBIND_SRC})

TARGET_LINK_LIBRARIES(rbind_framehelper ${BASE-TYPES_LIBS} ${BASE-TYPES_LDFLAGS} ${RBIND_OPENCV_LIBS} ${RBIND_OPENCV_LDFLAGS} frame_helper)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rbind.pc.in ${CMAKE_CURRENT_BINARY_DIR}/rbind_framehelper.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rbind_framehelper.pc DESTINATION lib/pkgconfig)

install(TARGETS rbind_framehelper LIBRARY DESTINATION lib)
install(FILES types.h operations.h conversions.hpp DESTINATION include/${PROJECT_NAME}/${DIR})

if (EXISTS ${PROJECT_SOURCE_DIR}/extern.rbind)
    install(FILES ${PROJECT_SOURCE_DIR}/extern.rbind DESTINATION include/${PROJECT_NAME}/${DIR})
endif()

if (EXISTS ${PROJECT_SOURCE_DIR}/config.rbind)
    install(FILES ${PROJECT_SOURCE_DIR}/config.rbind DESTINATION include/${PROJECT_NAME}/${DIR})
endif()
