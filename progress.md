# File to track progress for inception project and to list the next to dos

## To dos
[x] read through subject and take notes of all the new concepts
[] define unknown concepts and gather information
	[x] new functions
	[] create short test programms to understand new external functions
	[] new concepts
		[] video on TCP https://www.youtube.com/watch?v=F27PLin3TV0
[] reread subject with new information
[x] look up RFC 2810–2813 (updated IRC protocol specifications)
[x] IRC architecture
	- https://www.rfc-editor.org/rfc/rfc2810.html
[x] IRC Server Protocol
	- https://www.rfc-editor.org/rfc/rfc2813
[x] IRC client Protocol
	- https://www.rfc-editor.org/rfc/rfc2812
[x] Channel Management
	- https://www.rfc-editor.org/rfc/rfc2811
[] install client and connect to an IRC server and play around
	- decided to use irssi
	- https://modern.ircdocs.horse/#connection-registration
		[] read this documentation for the correct protocol between the server and irssi
[x] create a general structure
[x] divide project
	- for two people
		1. Handle Server, Clients and Channel state
		2. Parse commands and carry out commands -> affect Channel state
	- for three people
		1. Handle, Server
		2. parsing
		3. Channel State

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

## ====== Day 15 == 24.07 ====== (1h)
- worked on sending messages in channels
	- needs proper parser logic to handle incoming messages from irssi