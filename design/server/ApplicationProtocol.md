#Application Protocol
Application Protocol communicate with Message. Message contains Operation and contents needed.

## Structure of Message
-|-|-
no.|content|Description
1st-2nd bytes|length of the message not including the first two bytes|BigEndian

3rd byte|opetion type|message, join room, so on.
4th-nth|content|NULL

##Operation

### Register
code:  1 
content:
    three strings separated by '\n'
    - account
    - passwd
    - nickname

response:
    SUCCESS: 
        

    FAILUE:
    - reason



### Login

### Quit

### GetRooms

### BuildRoom

### CloseRoom

### ChangeRoomAdmin

### JoinRoom

### QuitRoom

### SendMessgeInRoom

### GetUsersInRoom

### PrivateMessage

### KickUser

