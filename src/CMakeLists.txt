
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
include_directories(applicationprotocol buffer converter common database server)

add_definitions(-std=c++11 -Wall -pthread -lcrypto -lssl -levent_openssl)
set (SRC_SERVER applicationprotocol/applicationprotocol.cpp buffer/buffer.cpp converter/converter.cpp database/database.cpp server/server.cpp)
set (SRC_CLIENT client/client.cpp)
add_executable(server ${SRC_SERVER})
add_executable(client ${SRC_CLIENT})
target_link_libraries(server event)
target_link_libraries(server ssl)
target_link_libraries(server event_openssl)
target_link_libraries(server crypto)

target_link_libraries(client event)
target_link_libraries(client pthread)
target_link_libraries(client ssl)
target_link_libraries(client event_openssl)
target_link_libraries(client crypto)


