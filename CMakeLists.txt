cmake_minimum_required(VERSION 3.1.0)

project(kddv VERSION 1.0.0 LANGUAGES CXX)

option(ENABLE_QT "Build with Qt GUI" ON)
option(ENABLE_NCURSES "Build with Ncurses TUI" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(PkgConfig)
pkg_search_module(JSONCPP jsoncpp)
pkg_search_module(LIBTINS libtins)

add_subdirectory(external/SOEM)

include_directories(
    include
    external/soem
    external/soem/osal
    external/soem/osal/linux
    external/soem/oshw/linux
    ${JSONCPP_INCLUDE_DIRS}
    ${LIBTINS_INCLUDE_DIRS}
)

if (ENABLE_QT)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)

    add_executable(kddv-gui
        src/gui_main.cpp
        src/ethercat_data_source.cpp
        src/ethercat_master.cpp
        src/packet_sniffer.cpp
        src/kelo_drive_slave.cpp
        src/robile_battery_slave.cpp
        src/kelo_bms_slave.cpp
        src/zmq_publisher
        src/gui.cpp
        include/gui.h
    )

    target_link_libraries(kddv-gui
        Qt5::Widgets
        soem
        zmq
        ${JSONCPP_LIBRARIES}
        ${LIBTINS_LIBRARIES}
    )
endif(ENABLE_QT)

if (ENABLE_NCURSES)
    add_executable(kddv-tui
        src/tui_main.cpp
        src/ethercat_data_source.cpp
        src/ethercat_master.cpp
        src/packet_sniffer.cpp
        src/kelo_drive_slave.cpp
        src/robile_battery_slave.cpp
        src/kelo_bms_slave.cpp
        src/zmq_publisher
        src/tui.cpp
        include/tui.h
    )

    target_link_libraries(kddv-tui
        soem
        zmq
        ncurses
        ${JSONCPP_LIBRARIES}
        ${LIBTINS_LIBRARIES}
    )
endif(ENABLE_NCURSES)

add_executable(generate_config_file
    src/generate_config_file.cpp
)

target_link_libraries(generate_config_file
    soem
    ${JSONCPP_LIBRARIES}
)
