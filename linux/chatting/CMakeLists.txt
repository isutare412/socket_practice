cmake_minimum_required(VERSION 3.13.4)

project("Chatting")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRC_DIR src)
set(COMMON_DIR ${SRC_DIR}/common)
set(SERVER_DIR ${SRC_DIR}/server)
set(CLIENT_DIR ${SRC_DIR}/client)

set(COMMON_SRC
    ${COMMON_DIR}/network/tools.cpp
    ${COMMON_DIR}/network/socket.cpp
    ${COMMON_DIR}/network/poll.cpp
    ${COMMON_DIR}/network/serialize.cpp
    ${COMMON_DIR}/thread/thread_pool.cpp
    ${COMMON_DIR}/thread/thread_task.cpp
    ${COMMON_DIR}/thread/semaphore.cpp
    ${COMMON_DIR}/packet.cpp
    ${COMMON_DIR}/session.cpp
)

set(SERVER_SRC
    ${SERVER_DIR}/session_manager.cpp
    ${SERVER_DIR}/chat_server.cpp
    ${SERVER_DIR}/client_handler.cpp
    ${SERVER_DIR}/server_main.cpp
    ${SERVER_DIR}/client_session.cpp
)

set(CLIENT_SRC
    ${CLIENT_DIR}/client_main.cpp
)

include_directories(src)
add_executable(server ${COMMON_SRC} ${SERVER_SRC})
add_executable(client ${COMMON_SRC} ${CLIENT_SRC})
