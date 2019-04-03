cmake_minimum_required (VERSION 2.6)

project (frozen_plus_fff)
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(lib)
# ncurses library
#find_package(Curses REQUIRED)
#include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_BUILD_TYPE Debug)
 
add_executable(run_test unittest/test_both_even.c 
src/both.c 
src/frozen.c 
src/presenter.c 
src/vista.c
src/login_service.c
src/tcp_socket_client.c
src/udp_socket_client.c
src/satellite_service.c
)
#ncurses library
#target_link_libraries(run_test ${CURSES_LIBRARIES})