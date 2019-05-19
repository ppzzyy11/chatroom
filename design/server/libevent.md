event_based
event types:

1. accpet_cb
    Do nothing.
    <!--Tell the "converter" new client in. -->
    <!--Passing parameters:-->
        <!--none.-->

2. client_read_cb
    A client send messages to the server.
    Pass parameters to the "converter": bev, vector<unsigned char>.
    Get return vector<pairt<bev*,vector<char>>> (send WHAT vector<char> to WHICH bev*)

3. client_write_cb
    Nothing to do. Output some log maybe.

4. client_event_cb
    A client close connection.

