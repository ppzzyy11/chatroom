# ChatRoom

This is a chatroom based libevent.

## Features:
1. Users with account, password, nickname.
2. Rooms with Admin, Blacklist, password and broadcast messages.
3. Offline messags would be sent to the receiver once the user gets online.
4. Database write to the disk every 10 seconds by default.
5. Dual-verified ssl.(All clients using the same .crt and .key)
6. Based on libevent.


## Run

    sudo apt install openssl libevent 
    mkdir build
    cd build 
    cmake ..
    make

## Directorires

    ./src/applicationprotocol/
    The Costomed Application Protocol.

    ./srr/buffer/
    A buffer than conver bytes into messages of the Customed Application Protocol.

    ./src/client/
    A client interacts with commands.

    ./src/common/
    Logging's implement and Datastructure's defination.

    ./src/converter/
    A converter connects the Application Layer and the libevent Layer, where the offline messages will be stored. The Converter also maintains the mapping of TCP and APP.

    ./src/database/
    A database that is called by the Application Layer. Save to the save file every 10 seconds.

    ./src/server/
    A server based on libevent with Dual-verified ssl support.

    ./src/ssl/
    The certificates and key of the client, server and CA. "Self-Signed"

    ./src/test/
    Some test uses the google-test.

## Protocols Stack:
|APP|a Customed Application Protocol|
|-|-|
|SSL|bufferevent_ssl|
|TCP|libevent|

## Architecture:

See "Architecture.dia".

## Misc:
The diagrams in the design file are out of date. But I am too lazy to overwrite them.
