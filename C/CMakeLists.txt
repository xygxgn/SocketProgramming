cmake_minimum_required(VERSION 3.0.2)
project(threadPool)

add_compile_options(-std=c++11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
  ${CMAKE_CURRENT_LIST_DIR}/bin
)

include_directories(
  include
)

add_executable(server 
  ${CMAKE_CURRENT_LIST_DIR}/src/server.c
  ${CMAKE_CURRENT_LIST_DIR}/src/ThreadPool.c
)

target_link_libraries(
  server
  pthread
)

add_executable(client 
  ${CMAKE_CURRENT_LIST_DIR}/src/client.c
)

target_link_libraries(
  client
  pthread
)
