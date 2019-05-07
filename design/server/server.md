#Transport Protocol
Description:
    using libevent to use TCP.

Ability:
    Accept new TCP connecttion

#Application Protocol
Description: 
    encode Command to Transport Layer or deocode Transport to Command 

Ability:
    Register Command:
        - account 
        - passwd 
        - nickname

    Login Command:
        - account 
        - passwd

    List Rooms:

    Build Room:
        
    Change Admin:

    Join a Room:

    Send Message in a Room:

    List Users in a room:

    Send a Message privatly

    Kick one User

    Quit Room

#Application Logic 
Description: 
    Rooms---Room---Admin
                |
                ---Member(s)

    Users---User

##Rooms
Description: 
    array of Room
    
Ability:
    Return an array of Room
    One User Build a Room
    One User Join a Room
    One User Send Message in a Room
    Return an array of Users in a Room
    kick one user in a room

###Room(s)
Description:    
- Name 
- Description
- passwd(or not)
- Admin
- Array of members
- Black List

Ability:
    initilizer
    Set Admin
    User Join in
    One User Send Message in the Room
    Return Users in this Room
    kick one users in a room


####Admin:User
Description:
    kcik one users

####Member(s):User


##Users
Description:

Ability:
    User Register
    User Login:
        Authentication
    One Send Message someone private

###User
Description:
- account
- passwd 
- nickname

Ability:
    Send Message someone private
    Be sent private messge

#Data Storage


Ability:
    Register new user's information
    Get user's information
    Get List of Rooms
    Build a room
    Change Room Admin
    One User Join a Room
    Get List of Users in a Room 
    One User leave the room



<!--to optimize-->
#Log

