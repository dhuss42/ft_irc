# File to track progress for inception project and to list the next to dos

## To dos
[] update authentication process
	- currently it is waiting on the booleans to turn true but it should only check once respectively wether NICK, PASS, USER were successfull otherwise it will not delete the client object
		- results in not properly closing the connection when pass is incorrect
	- solution is to non-block authentication during newClient. The Client should be simply incldued in the list and the complete registration should only happen when all three commands are parsed and handled
		- PASS, NICK, USER should send a return value that indicated wether the command sent during registration was successfull. if one of them fails the user has to be disconected
		- send welcome message when connected
	[x] check how to handle disconnectClient from Server method
[] could be that more bugs occur where names have not been changed to lowercase resulting in segfaults
[] add @ for operator inside chat
	- the clients who are not operator see the @ but since the broadcast message does not send to the original sender his nick is displayed without @ in his chat
	- means that the operator needs a change in nick for that channel so it can be displayed in irssi
	-> operators are stored in two containers
		-> channel users
		-> channel operators
		-> when doing something channel specific check operators containers
		-> when doing something from outside check channel users
[x] server is slow at times -> RESOLVED
	- could be because of authentication loop
[x] when two users in the same channel have the same nick they are treated as the same, one client receives two messages and the other does not receive any
	[x] currently server is not handling two cliebnts with same nick. Either not allow it or suffixs "_" to the nick
[] test userlimit when set in channel
	-> inside the irssi Client it looks like the User joined a channel
	-> original irssi servers hinder from joining
[] test userLimit on Server
	[] think about implementing a cap for users to Join since the server is not allowed to shutdown
[] How Many Channels can 1 user join?
	- consider enforcing a limit for Channels
[] check for leaks again
	[x] free all channels when server stops
	[x] also Make methods for cleaning up clients and closing sockets and callthem in destructor instead of doing it all in destructor
[x] register on intra
[] test on Linux
[] eval Point
[] clean up
	[x] make some sort of order in files
	[x] look for functions that are not used
	[x] add 42 header everywhere
	[x] method descriptions
	[] non-member methods like in utils.cpp look up ruling in 42
	[] consier what to display during server run (not a lot since std::cout hinders performance)
	[x] delete Architecture branch
[] test strange printf input with nc -> segfaulted
[] maybe implement clientSendBuffer because if segfault
	- try to replicate segfault first
[] check for signals
[x] default constructor
[] what do we do when a Client connects and only sends part of the Authentication?
	-> PASS but not NICK and USER
	-> disconnect after timeout?
[] what happens when last Operator leaves channel but there are other clients still in the channel
	-> make one an operator?

## ====== Day 1 == 25.06 ======
- started reading subject (30min)

## ====== Day 2 == 01.07 ======
- watched videos on C++ Network Programming
	- https://www.youtube.com/watch?v=gntyAFoZp-E
		- is for windows but bottom line very similiar
		- Did the exercise at the end of the video and it worked
	- https://www.youtube.com/watch?v=sXW_sNGvqcU (26:18)
		- Exercises 1:
			- made it possible to send a message from client to server and display it in the server terminal
		- Exercise 2:
			- upon receiving a message from a client the server now sends an automatic reply back
		- Exercise 3:
			- allow an unlimited number of message to be typed into the client and each one should receive a confirmation
		- Exercise 4:
			- terminate the server and client if USER types EXIT into the client
- Defining functions that can be used for the project
	- https://medium.com/@ahmadesekak/setting-up-a-server-using-c-sockets-124e404593c9
		- socket(), bind(), listen(), accept()
- error when empty message

## ====== Day 3 == 02.07 ======
- defined unknown functions mentioned in the subject
	- epoll()
		- https://www.youtube.com/watch?v=O-yMs3T0APU
	- lseek()
		- https://www.youtube.com/watch?v=20Gs3UpP0t4
- worked on some example functions
- added new concept:
	- Endianness
- thought about a first general structure

## ====== Day 4 == 03.07 ======
- fixed example functions (server) bow using poll and fcntl correctly
- defined unknown functions mentioned in subject
	- signals
		- introduction
			- https://www.youtube.com/watch?v=5We_HtLlAbs&list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY&index=16&pp=iAQB
		- stopping and continuing execution of processes
			- https://www.youtube.com/watch?v=3MZjaZxZYrE&list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY&index=17
		- sigaction and why not to use signal
			- https://www.youtube.com/watch?v=jF-1eFhyz1U&list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY&index=19
- read  Internet Relay Chat: Architecture Documentation
	- https://www.rfc-editor.org/rfc/rfc2810.html
- read Internet Relay Chat: Server Protocol (skipped parts of 3 and 5 only server to server relevant I think),
	- https://www.rfc-editor.org/rfc/rfc2813#section-10

## ====== Day 5 == 04.07 ======
- read Client Protocol Document
	- https://www.rfc-editor.org/rfc/rfc2812
- worked on structure
	- worked on structure for the server object
	- worked on structure for the client objects
	- worked on structure for main
	- created directory structure and server related files
	- added Makefile
- thought about optional config file
	- possibility to alter configuration of server without altering code
	- requires additional file parsing

## ====== Day 6 == 06.07 ======
- read Channel Management
	- https://www.rfc-editor.org/rfc/rfc2811
- created structure for channel class
- finished first general structure (still needs details for parsing)
- added some information about the user commands required by the subject

## ====== Day 7 == 07.07 ======
- implemented error handling logic
- fixed Makefile
- implemeted parsing of args for port and password
	- further considerations to make for password (documented in server.cpp)
- started implementing server class including initialiser methods
	- Server constructor
	- initialising Server
		- socket(), fcntl() bind(), listen()

## ====== Day 8 == 08.07 ======
- worked on server Loop
	- it is now possible to connect with custom client using the same port number
	- loop handle poll() return values if non negativ revents are considered
		- checks for new client connection
			- accepts connection
			- creates new client object(is currently empty)
			- stores client object together with corresponding fd in map
			- stores fd in vector of pollfds together with socket
		- currently only prints message for receiving message and connection lost (still need implementation)
	- listening socket is first in vector of pollfds and connecting client pollfds follow
- more error handling
- some documentation of the methods

## ====== Day 9 == 09.07 ====== (1h)
- implemented receivingMsg methods that fills teh buffer until a certain char is encountered
	- needs to be changed to the char sequence that is needed for IRC
- implemented sendMSg method
- implemented basic password authentication just for testing the logic

## ====== Day 10 == 12.07 ====== (1h)
- worked on client connections and receiving messages
	- currently multiple clients can be connected
	- multiple messages can be sent
	- problem when disconnecting client with crtl + c after entering the password, but not before entering password
- receiveMsg function needs to be updated to break loop at correct char sequence
	- this is a bigger thing that needs to be tackled

## ====== Day 11 == 17.07 ====== (7h)
- fixed receiving messages
- fixed closed connection on client side
- trying connection with irssi
	- https://modern.ircdocs.horse/#connection-registration
		- documentation for the connection
	- can connect
	- sends CAP LS 302
		- server capabilities makes it possible to connect when server does not posses the capabilities
	- server responds with no capabilities
	- client sends JOIN and CAP END
- !!!! connecting irssi and ircserv works now !!!!
	- irssi -c 127.0.0.1 -p 12345 -n irssitest -w hallo
		- irssi -> launches the IRC client
		- -c 127.0.0.1 Connects to server at IP localhost
		- -p 12345 is the port
		- -n sets nickname
		- -w provides password
	- sending the password, nick and user name work
	- server also sends
		- MODE +i
		- WHOIS
		- PING
- next steps should be cleaning up the code and properly freeing resources when connections are closed

## ====== Day 12 == 18.07 ====== (7h)
- worked on documentation of Server class
- small fixes for client class
- testing with valgrind
	- Container:
		- docker build -t valgrind .
		- docker run -p 12345:12345 -it --rm -v /Users/david/coding/42/circle_5/ft_irc:/app valgrind
		- go into app folder
	- ircserv
		- _addr.sin_addr.s_addr = htonl(INADDR_ANY);
	- connect normally with irssi
	- needs signal handling for correct destructor call for server
		- handled SIGINT
			- currently 1 leak when client disconnects with quit
				- solved
			- 5 still reachables too
			- leak when there is a bind error

## ====== Day 12 == 21.07 ====== (40min)
- refactored sending and receiving messages so that it is part of client class now not server class

## ====== Day 13 == 22.07 ====== (3h)
- handles leaks and still reachables
	- main problem was exiting when pressing ctrl + c instead of returning in main

## ====== Day 14 == 23.07 ====== (6h)
- added setsockopt so there is no time out for bind()
- added minor things for Channels, Clients
- added basic methods and attributes for Channels class
- worked on server -> Channel logic
	- needs further work also handling adding users

## ====== Day 15 == 24.07 ====== (2h)
- worked on sending messages in channels
	- needs proper formating for being displayed in channels

## ====== Day 16 == 10.09 ======
- returned after 6 weeks, need to identify what is not working at the moment
- Problem with communication inside channels, two users can join the same channel but do not receive the messages
- clients connecting seem to skip the first letter of their name
	- could be because of terrible presudo parser
- First commands needed to be handled by parser should be -> see Day 11 https://modern.ircdocs.horse/#connection-registration
	- CAP LS 302 -> for server capabilities
	- JOIN
	- PASS
	- NICK

## ====== Day 17 == 16.09 ====== (6:45h)
[x] get irssi running on campus
[x] play around to get back into logic
[x] fix naming bug in pseudoparser
- think about the logic in creating a new channel
- work on sending the message back to the server
	- currently not displaying the message in irssi because of the way the server is replying to irssi
	- need to read upon the correct way for it to respond
- need proper sendmsg method
	[x] implement username
	[x] hostname
	- correct string formating for reply to be acknoledged by irssi

## ====== Day 17 == 17.09 ====== (6:30h)
[x] look into correct string formating when sending replies
[x] look into broadcasting messages in channels
[x] make pseudo-code for function verifying nicknames
[x] make an overview of things that still need do be implemented

Channel Stuff
- reminder to implement delete channel when last member has left, except "!"
- make method creating different channel types
[x] split the logic so that the parser calls this method and it's not part of the parser
[x] implement changing the topic of a channel
[x] consider if the boolean is set for topic only op
- think about how to implement channelop prefix '@'

Messaging
- one to one communication Client A -> Server -> Client B
- One to many
	- Channel
		- single client in Server Client A -> Server
		- two clients like one to one communication
		- three clients goes to all clients
	- To Host/Server Masl
		- not sure if we need to do this
	- To A List
		- not sure if we need to do this
- One to All
	- broadcast to every client
	- broadcast to ever server
- Client to Server
	- commands that change the state of information (channel membership, channel modes, user status, etc.)

Casemapping
- when new clients and channels are registered check wether the name exists already (Dan, dan)
	[x] done for channels
	[] currently not sure if correctly done for clients

- currently server can send some messages back and forth but seems to hang after some messages and sometimes the server segfaults

## ====== Day 18 == 18.09 ====== (6h)
- though about file separation and simplest solution is to make Parsing a separate class with it's own files
	- should take a client n constructor
[x]	handle NICK Command
	[x] checking the string for legal chars
		-> utils.cpp
	[x] check if the nick is already in use
		-> Server.cpp
	[x] store the nick in lowercase inside the container when a new client is registered
		-> change in Server.cpp

[] handle PASS Command
	[x] getPassword for server
		-> Server.cpp
	[x] consider storing password in class as const as it may not be changed
		-> change in Server.hpp
	[] close connection
		-> currenty authentification Method is not sufficient
			- it is waiting on the booleans to turn true but it should only check once respectively wether NICK, PASS, USER were successfull otherwise it will not delete the client object
			- results in not properly closing the connection when pass is incorrect
			- solution is to non-block authentication during newClient. The Client should be simply incldued in the list and the complete registration should only happen when all three commands are parsed and handled
			- PASS, NICK, USER should send a return value that indicated wether the command sent during registration was successfull. if one of them fails the user has to be disconected

[] handle USER Command
	[] verify username
		- musst not be empty
		- printable non-whitespace chars
		- exclude @
	[] verify realname
		- may contain SPACE char and is prefixed with colon by irssi
		- may be empty
	[x] getters and setters for username/hostname/realname

[] handle JOIN COMMAND
	[x] getKey/password
	[x] check if joining is possible (inv/pw/usrlimit)
	[x] add to channel user list
		-> but something is buggy here / maybe it's privmsg though
	[x] get topic
	[x] getJoined users
	[x] check mode section
	[x] special parameter for PARTING all channels at once

[x] handle PART COMMAND
	[x] remove Client from Channel container
		-> Channel::removeUser(Client* client)
	[x] check: if client was operator and Parts and rejoins is he still operator?
		-> no
	[x] check if channel has 0 members if so delete channel object
		-> do inside the handler

[x] handle KICK
	[x] check if user is operator
		-> void	Channel::kickUser(Client* kicker, std::string kicked)
	[x] remove User from Channel
	[] optional kick limit

[x] handle INVITE
	[x] nickname to be invited and channel to be invited
	[x] check if the user exists on server
	[x] check wether the inviting user is part of channel
	[x] check wether the inviting user is an operator
	[x] check wether the invited user is already part of the channel

[x] handle TOPIC COMMAND
	-> changeTopic() in Channel.cpp

[x] handle MODE (only channel modes)
	[x] check if channel exists on network
	[x] when no string is given returns current channel modes
		-> test this with irssi
	[x] returns creation time of the channel
		-> const time_t Channel::getCreationTime(void) const
	[x] if target is channel, user sending command musst have appropriate channel priviledges
	[x] check Types
		- Type A
		- Type B
			k	-> password
			o	-> nick
		- Type C
			+l needs parameter
			-l not
		- Type D
			i and t -> toggles

## ====== Day 19 == 19.09 ====== ()
[x] created getters and setters for Server related stuff
[x] wrote some basic functions for commands (all ready except privmessage)
[x] handle PRIVMSG
	[x] between users
	[x] message to channel
		[] if target is a channel then it is possible to send only to certained prefixed members
			-> in our case only operators
	[] when starts with $ the message is sent to all clients on the server
[x] fixed bug inside PRIVMSG where capital letter cannels where searched wrong inside getChannel
[x] though about logic how to fix authentication process or make a method that can be implemented as soon as the necessary cmds are handle by parser
[x] tested sending larger messages without all the Debug information and server does not freez
- server is a bit slow when responding
	[x] fixed some delay because poll had a sleep of 1 second -> not 100ms
	-> getting pull error now when pressing SIGINT
[x] verified a few things irssi does: no creationTime and empty mode command is displayed as "+"
	-> noticed that irssi itself is also a bit laggy
[x] -> checking core DUMP (ctrl + \)
	-> quits the server now
[] ctrl + z results in chaos

## ====== Day 20 == 22.09 ====== (4h)
[x] handle ctrl + z and fg properlys
[x] handle ctrl + z and starting another server properly
	[x] handle the new bind error now
[x] fix bug with poll error when pressing ctrl + c/q 
[x] getChannels -> returns all channels as a vector
[x] addUser as a boolean
[x] fixed circular include bug
[x] improve error handlings, fatal and non-fatal errors
	[x] getter and setter for should Exit
	[x] pass pointer to server when Error is called
	-> is better but not optimal

## ====== Day 21 == 22.09 ====== (6h)
- worked on server speed

## ====== Day 22 == 24.09 ====== (4h)
[x] realised "laggy" speed comes from irssi not our server
	- compare in eval to normal networks with irssi
[x] testing multiple Client connections to the server
	- issues when chatting with 50 people in one channel
[x] User Connecting with Nick who has been part of the channel and then disconnected and connected again with same nick resulted in this: ----> RESOLVED
	[DEBUG] CAP:
	[DEBUG] Capability negotiation completed
	[DEBUG] JOIN:
	[DEBUG] #test exists already, not creating a new channel
	[DEBUG] Client is already part of the Channel!
	Segmentation fault: 11
	[x] Need to handle situation in which Nick is already taken -> RESOLVED
	[x] Need to handle removal out of all Channels to which user was connected -> RESOLVED

- Connecting with 50 testusers in 1 Channel each sending a message every 5 seconds
	[DEBUG] current reply: testnick14!tError: send error
			Default error
	Error: send error
			Default error
	Error: send error
			Default error
	Error: send error
			Default error
	Error: send error
			Default error
	Error: send error
			Default error
	Error: std::bad_alloc
	Error: std::bad_alloc
	Error: std::bad_alloc
	Error: std::bad_alloc
	Error: std::bad_alloc
	Error: std::bad_alloc
	Segmentation fault: 11
		-> maybe make a sendBuffer for every client

## ====== Day 23 == 25.09 ====== (5h)
[x] deleted ServerDavid.cpp & tmp.cpp
[x] free all channels when server stops
[x] also Make methods for cleaning up clients and closing sockets and callthem in destructor instead of doing it all in destructor
[x] check how to handle disconnectClient from Server method
[x] add 42 header everywhere

[] clean up
	[x] make some sort of order in files
		-> did in Server.cpp and Server.hpp
		-> did in Client.cpp and Client.hpp
		-> started in Channel.cpp and Channel.hpp

	[] look for functions that are not used
		-> did in Server.cpp and Server.hpp
		-> did in Client.cpp and Client.hpp
		-> started in Channel.cpp and Channel.hpp
	[x] add 42 header everywhere
	[x] method descriptions
		-> did in Server.cpp
		-> did in Client.cpp
		-> started in Channel.cpp and Channel.hpp

For leak check at School start Dockerfile in directory
- docker build -t valgrind .
- docker run -p 6667:6667 -it --rm -v $PWD:/app valgrind
- valgrind --leak-check=full ./ircserv 6667 123
- run ConnectionTest.sh in separate Terminal with port 6667 and pass 123
--> currently no leaks for 15 users connectin to one Channel and writing messages
--> couldn't find any leaks
--> invalid read for USER and PASS with invalid arguments but is related to them not being handeld yet

## ====== Day 24 == 26.09 ====== ()
- wrote getter for Channel Parameters
- did further cleanup
- deleted Architecture branch
- added Error responses for Channel::addUser()
- clean up
- handled constructors for Client/Server/Channel

## ====== Day 25 == 27.09 ====== ()
- description for Signals