#Application Protocol
Application Protocol communicate with Message. Message contains Operation and contents needed.

## Structure of Message
-|-|-
no.|content|Description
1st-2nd bytes|length of the message not including the first two bytes|BigEndian

3rd byte|opetion type|message, join room, so on.
4th-nth|content|NULL
account first must
content follows.

(strings are separated by '\n')
##Operation

### Register
code:1 
content:
    - account
    - passwd
    - nickname

response:
    SUCCESS: 

    FAILUE:
    - reason



### Login

--------------------------------------------------------------------------------
ignore this "Login", the online and offline status are managed by the Converter.
--------------------------------------------------------------------------------

code:2
content:
    - account
    - passwd

response:
    SUCCESS:

    FAILUE: 
    - reason

### Quit
--------------------------------------------------------------------------------
ignore this "Login", the online and offline status are managed by the Converter.
--------------------------------------------------------------------------------
code:3
content: 
    - account 
    - passwd

response:
    SUCCESS:

    FAILUE: 
    - reason

### GetRooms
code:4
content: 
    - account 

response:
    SUCCESS:
    - room lists separated by '\n'
        - room id 
        - room name 
        - room admin's nickname
        - room description

    FAILUE: 
    - reason

### BuildRoom
code:5
content: 
    - user account
    - room id
    - room name 
    - room description
    - room paswd 

response: 
    SUCCESS: 

    FAILUE:
    - reason

### CloseRoom
code:6
content: 
    - user account 
    - room id

response: 
    SUCCESS: 

    FAILUE:
    - reason

### ChangeRoomAdmin
code 7
content:
    - oldadmin account 
    - room id 
    - new admin account

response: 
    SUCCESS: 

    FAILUE:
    - reason
### JoinRoom
conde 8 
content: 
    - user account
    - room id

### QuitRoom
code 9 
content:
    -user id
    -room id

### SendMessgeInRoom
code 10
content: 
    - user account
    - room id
    - message 

### GetUsersInRoom
code 11
content:
    - room id

response:
    - cause

### PrivateMessage
code 12
content:
    - sender account
    - received account
    - message

### KickUser
code 13
content:
    - kicker account
    - room id
    - kicked account

### SUCCESS
content:
    - success string

### FAILURE
content:
    - cause string



### GetRoom
code:16
content: 
    - account 
    - room id

response:
    SUCCESS:
    - room id 
    - room name 
    - room admin's nickname
    - room description

    FAILUE: 
    - reason

