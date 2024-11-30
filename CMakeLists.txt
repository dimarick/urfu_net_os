cmake_minimum_required(VERSION 3.16)
project(net_os)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

add_executable(lr2_daemon lr2/daemon.c)
add_executable(lr3_ipc lr3/ipc.c)
add_executable(lr4_socket_stream_client lr4/client.c)
add_executable(lr4_socket_stream_server lr4/server.c)
add_executable(lr5_socket_datagram_client lr5/client.c)
add_executable(lr5_socket_datagram_server lr5/server.c)

install(TARGETS lr2_daemon RUNTIME DESTINATION bin)
install(TARGETS lr3_ipc RUNTIME DESTINATION bin)
install(TARGETS lr4_socket_stream_client RUNTIME DESTINATION bin)
install(TARGETS lr4_socket_stream_server RUNTIME DESTINATION bin)
install(TARGETS lr5_socket_datagram_client RUNTIME DESTINATION bin)
install(TARGETS lr5_socket_datagram_server RUNTIME DESTINATION bin)
install(PROGRAMS lr2/S09lr2-daemon-init.sh DESTINATION /etc/rc5.d)