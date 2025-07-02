# File to track progress for inception project and to list the next to dos

## To dos
[] read through subject and take notes of all the new concepts
[] define unknown concepts and gather information
[] create short test programms to understand new external functions
[] reread subject with new information
[] Exercise 5:
		- Mdofiy the server so that each new connection is managed by a thread
			- within main you will need to a loop that accepts connections and creats a new thread, passing the accepted socket to the thread function

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
