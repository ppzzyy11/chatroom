cmake_minimum_required (VERSION 2.8)

# projectname is the same as the main-executable
project(chatroom)

add_subdirectory(src)

file(COPY ${CMAKE_SOURCE_DIR}/src/ssl/server.key DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/src/ssl/server.crt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/src/ssl/client.key DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/src/ssl/client.crt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/src/ssl/CA.key DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/src/ssl/CA.crt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
