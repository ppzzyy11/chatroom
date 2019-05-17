# Converter
## Description:
Call interface of libevent and application layer, receive the TCP stream and convert it into Application Message 

## Ablitities
1. Call interfaces of libevent.
2. Call interfaces of application layer.
3. Receive libevent's stream.
4. Receive application's message.
5. Convert TCP stream into messages of application layer.
6. Send messages
7. Deal with the online and offline status
8. When one user online, send him messages sent to him when offline.
9. If message except the Authentation sent to AppLayer, we assume that the user is admitted.
