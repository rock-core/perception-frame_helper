find_package(Rock)
include(RockRuby)

find_package(Gem COMPONENTS rice)
pkg_check_modules(TYPELIB "rtt_typelib-${OROCOS_TARGET}")
pkg_check_modules(TYPELIBRUBY "typelib_ruby")

if (TYPELIB_FOUND AND TYPELIBRUBY_FOUND AND GEM_FOUND)
    rock_ruby_rice_extension(frame_helper_ruby SHARED
        SOURCES FrameHelperRice.cpp
        DEPS_PKGCONFIG rtt_typelib-${OROCOS_TARGET} typelib_ruby)
    target_link_libraries(frame_helper_ruby frame_helper ${GEM_LIBRARIES})
else()
    if (NOT TYPELIB_FOUND)
        message(STATUS "FrameHelper: Cannot find typelib => disable ruby binding")
    elseif (NOT TYPELIBRUBY_FOUND)
        message(STATUS "FrameHelper: Cannot typelib's ruby bindings => disable ruby binding")
    elseif (NOT GEM_FOUND)
        message(STATUS "cannot find the rice gem, the vizkit-ruby bridge is disabled")
    endif()
endif()

