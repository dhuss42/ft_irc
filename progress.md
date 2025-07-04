# File to track progress for inception project and to list the next to dos

## To dos
[x] read through subject and take notes of all the new concepts
[] define unknown concepts and gather information
	[x] new functions
	[] create short test programms to understand new external functions
	[] new concepts
[] reread subject with new information
[] look up RFC 2810–2813 (updated IRC protocol specifications)
[x] IRC architecture
	- https://www.rfc-editor.org/rfc/rfc2810.html
[x] IRC Server Protocol
	- https://www.rfc-editor.org/rfc/rfc2813
[x] IRC client Protocol
	- https://www.rfc-editor.org/rfc/rfc2812
[] Channel Management
	- https://www.rfc-editor.org/rfc/rfc2811
[] install client and connect to an IRC server and play around
	- https://www.irchelp.org/faq/new2irc.html 
[] create a general structure
[] divide project

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