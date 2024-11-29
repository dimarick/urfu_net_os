cmake_minimum_required(VERSION 3.16)
project(net_os)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Release)

add_executable(lr2_daemon lr2/daemon.c)
add_executable(lr3_ipc lr3/ipc.c)

install(TARGETS lr2_daemon RUNTIME DESTINATION bin)
install(TARGETS lr3_ipc RUNTIME DESTINATION bin)
install(PROGRAMS lr2/S09lr2-daemon-init.sh DESTINATION /etc/rc5.d)