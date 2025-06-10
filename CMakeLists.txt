# cmake -G Ninja -B build -DPTP_INCLUDE_EXAMPLES=ON -DPTP_INCLUDE_VCAM_TEST=ON
cmake_minimum_required(VERSION 3.15)

project(pict)

option(PTP_CANON_ADV "Include implementation for advanced Canon features" ON)
option(PTP_STUFF "Include extra functions for debugging/testing" ON)
option(PTP_DEFAULT_LOGGING "Include default log/panic/error implementation" ON)
option(PTP_INCLUDE_CLI "Include CLI program" OFF)
option(PTP_INCLUDE_EXAMPLES "Include examples" OFF)
option(PTP_INCLUDE_VCAM_TEST "Include vcam test" OFF)
option(PTP_INCLUDE_BIND "Include optional bind.c" ON)
option(PTP_NO_USB "Compile src/no_usb.c in place of libusb/libwpd" OFF)
option(PTP_NO_TCP "Compile no_ip.c in place of TCP code" OFF)
option(PTP_INCLUDE_WINDOWS_TEST "Pull in libwpd and test windows compilation" OFF)

set(PICT_CORE
    src/operations.c
    src/packet.c
    src/enums.c
    src/data.c
    src/enum_dump.c
    src/lib.c
    src/canon.c
    src/liveview.c
    src/ml.c
    src/conv.c
    src/generic.c
    src/transport.c
)
if(NOT PTP_NO_TCP)
    list(APPEND PICT_CORE src/ip.c)
else()
    list(APPEND PICT_CORE src/no_ip.c)
endif()
if(PTP_INCLUDE_BIND)
    list(APPEND PICT_CORE src/bind.c)
endif()
if(PTP_CANON_ADV)
    list(APPEND PICT_CORE src/canon_adv.c)
endif()
if(PTP_STUFF)
    list(APPEND PICT_CORE src/stuff.c)
endif()
if(PTP_DEFAULT_LOGGING)
    list(APPEND PICT_CORE src/log.c)
endif()
if(PTP_NO_USB)
    set(PLATFORM_LIB_FILES src/no_usb.c)
endif()

if(UNIX AND NOT PTP_NO_USB)
    if(TARGET usb-1.0)
        list(APPEND PICT_LIBS usb-1.0)
    else()
        find_package(PkgConfig)
        if(PKG_CONFIG_FOUND)
            pkg_check_modules(LIBUSB IMPORTED_TARGET GLOBAL libusb-1.0)
            if(LIBUSB_FOUND)
                list(APPEND PICT_LIBS PkgConfig::LIBUSB)
            else()
                message(FATAL_ERROR "install libusb")
            endif()
        else()
            option(LIBUSB_BUILD_SHARED_LIBS "" OFF)
            option(LIBUSB_INSTALL_TARGETS "" OFF)
            include(FetchContent)
            FetchContent_Declare(libusb GIT_REPOSITORY https://github.com/libusb/libusb-cmake.git GIT_TAG v1.0.28-0 GIT_SHALLOW 1)
            FetchContent_MakeAvailable(libusb)
            list(APPEND PICT_LIBS usb-1.0)
        endif()
    endif()
    list(APPEND PICT_CORE src/libusb.c)
elseif(WIN32 AND NOT PTP_NO_USB)
    list(APPEND PICT_CORE src/libwpd.c)
    if(NOT TARGET wpd_static)
        include(FetchContent)
        FetchContent_Declare(libwpd GIT_REPOSITORY https://github.com/petabyt/libwpd.git GIT_TAG master GIT_SHALLOW 1)
        FetchContent_MakeAvailable(libwpd)
    endif()
    set(PICT_LIBS wpd_static)
endif()

add_library(libpict STATIC ${PICT_CORE} ${PLATFORM_LIB_FILES})
target_compile_options(libpict
    PRIVATE -Wall -Wshadow -Wcast-qual -Wpedantic -Werror=incompatible-pointer-types -Werror=incompatible-pointer-types -Werror=deprecated-declarations -Wstrict-aliasing=3 -Werror=narrowing -DVERBOSE
)
target_include_directories(libpict PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(libpict PUBLIC ${PICT_LIBS})
set_target_properties(libpict PROPERTIES OUTPUT_NAME pict) # libpict.a

if(PTP_INCLUDE_CLI)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBPCAP REQUIRED IMPORTED_TARGET GLOBAL libpcap)
    add_executable(ptp
        src/cli.c
        src/dec.c
    )
    target_include_directories(ptp PUBLIC ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(ptp libpict PkgConfig::LIBPCAP)
    install(
        TARGETS ptp
    )
endif()

macro(add_example exe_name source)
    add_executable(${exe_name} ${source})
    target_include_directories(${exe_name} PUBLIC ${PROJECT_SOURCE_DIR}/src)
    if(WIN32)
        target_link_libraries(${exe_name} libpict -lpthread -lurlmon -luuid -lws2_32)
    else()
        target_link_libraries(${exe_name} libpict)
    endif()
endmacro()

if(PTP_INCLUDE_EXAMPLES)
    add_example(info examples/info.c)
    add_example(optest examples/optest.c)
    add_example(directprint examples/directprint.c)
    add_example(eos examples/eos.c)
    add_example(evtest examples/evtest.c)
    add_example(live examples/live.c)
    add_example(ml examples/ml.c)
    add_example(pktest examples/pktest.c)
    add_example(storage examples/storage.c)
    add_example(usb examples/usb.c)
    add_example(wifi examples/wifi.c)
endif()

if(PTP_INCLUDE_VCAM_TEST)
    if(EXISTS "${CMAKE_SOURCE_DIR}/../vcam")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vcam ${CMAKE_CURRENT_BINARY_DIR}/vcam)
    else()
        include(FetchContent)
        FetchContent_Declare(vcam GIT_REPOSITORY https://github.com/petabyt/vcam GIT_TAG master GIT_SHALLOW 1)
        FetchContent_MakeAvailable(vcam)
    endif()

    add_executable(test test/test.c test/data.c)
    target_link_libraries(test libpict usb-vcam)
endif()
