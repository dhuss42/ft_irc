# new concepts

- Internet Relay Chat (IRC)
	-  text-based communication protocol on the Internet
	- real-time messaging (private and public)
	- user can exchange direct messages and join group channels
- IRC server
	- IRC servers are connected togther to form a network
- IRC client
	- connect to IRC servers in order to join channels
- solid standard protocols
	-


- I/O operations must be non-blocking
	-
- IRC client as reference
	-
- operators in IRC

# new external functions

## Server Functions
- socket()
	- creates a socket
	- <sys/socket.h>
	- int socket(int domain, int type, int protocol);
		- returns file descriptor assigned to the socket
		- returns -1 if there is an error
	- domain: the protocal the socket will use for communication
		- AF_UNIX / AF_LOCAL is used for local communication
		- AF_INET and AF_INET6 used for internet protocol IPV4, IPV6 <- needed for tcp and udp
	- type: wether the connection will be connection-less or persistent
		- SOCK_STREAM - two-way reliable communication TCP <-
		- SOCK_DGRAM - Connection-less, unreliable (UDP)
	- Protocol: usually only one protocal available for each type
		- 0 (?) / IPPROTO_TCP (?)
- bind()
	- assign IP address and port to the socket
	- int bind(int sockfd, const sockaddr *addr, socklen_t addrelen);
		- returns -1 on error
		- returns 0 onsuccess
	- sockfd:
		- file descriptor we want to assign an address to (socket that returns from socket function)
	- addr:
		- struct used to specify the address and port we want to assign to the socket.
		- sockaddr_in as we are using IP
			- #include <netinet/in.h>
			struct sockaddr_in {
				sa_family_t    sin_family; /* address family: AF_INET */
				in_port_t      sin_port;   /* port in network byte order */
				struct in_addr sin_addr;   /* internet address */
				};
		- addrelen:
			- size of addr
- listen
	- marks a socket as passive. The socket will be used to accept incoming connections
	- int listen(int sockfd, int backlog);
		- returns -1 in case of an error
		- 0 on success
	- sockfd:
		- is the file descriptor of the socket
	- backlog:
		- maximum number of connections that will be queued before connection stat is being refused
- accept
	- pauses the execution of the server until the client connects to it
	- extracts an element from a queue of connections (created by listen) for a socket
	- int accept(int sockfd, sockaddr *addr, socklen_t *addrlen);
		- returns -1 on error
		- on success returns a fd for the connection
			- the accpeted socket is the connection between the client and the server
	- sockfd
		-> will remaining listinen for connections, separate to the accpeted socket
	- addrlen is now a pointer to the size of addr
- connect
	- initiate a connection on a socket
	- int connect(int sockfd, const struct sockaddr *addr,
                   socklen_t addrlen);
	- 75 seconds respond time
	- if successfull returns 0
	- if error returns error

send and recv both block execution until data has been sent and received respectively
- need to be set up in differing order for client and server
- server end start with receive function

- send
	- sends data on a connected socket
	- bidrectional between client and server
	- int send (int acceptedSocket, const char* buf, int len, int flags)
		- acceptedSocket: the connected socket
		- buf: data that we want to send
		- len: the length of the array
		- flags: optional set of flags
	- returns
		- -1 on error
		- number of bytes which were sent are returned
			- we want a non zero value to be successfull
	- cin.getline(buffer, len)
		- to not separate message by whitespaces

- recv
	- size_t recv(int socket, void *buffer, size_t length, int flags);
		- same as send
	- returns
		- -1 on error
		- number of bytes which were sent are returned
			- we want a non zero value to be successfull


- close
	- closes the file descriptors

- setsockopt
	- #include <sys/socket.h>
	- int setsockopt(int sockfd, int level, int option_name,
               const void *option_value, socklen_t option_len);
		- the socket file descriptor you created with socket()
		- the protocol level the option applies to
		- the specific option you want to set 
		- pointer to the value you want to set
		- size of the value you are passing
	- lets you configure options for a socket
		- Allowing address reuse (SO_REUSEADDR)
		- ...
	- returns 
		- -1 on error
		- 0 on success

- getsockname
	-
- getprotobyname
	-
- gethostbyname
	-
- getaddrinfo
	-
- freeaddrinfo
	-
- htons
	- pass portnumber and returns value in TCP/IP network byte order -> for the network to understand
	- have to pass port number to this function and store the retuns value in sockaddr_in struct .sin_port
	- Host To Network Short” → converts a 16-bit (short) number (like the port) to network byte order (big endian).
- htonl
	- “Host To Network Long” → converts a 32-bit (long) number (like an IPv4 address) to network byte order.
- ntohs
	-
- ntohl
	-
- inet_addr
	- Converts the IPv4 address string "127.0.0.1" directly into the required 32-bit network byte order integer.
	
- inet_to_a
	-

- signal
	-
- sigaction
	-
- lseek
	-
- fstat
	-
- fcntl only as follows: fcntl(fd, F_SETFL, O_NONBLOCK);
	-
- poll (or equivalent) (select, kqueue, epoll)
	-

# relevant concepts

## OSI Model (Open Systems into connectin)
- 7 Layers between highlevel software level (application layer) down to the physical level the hardware
- For a connection to take place etween two devices (for one device to send data to anothe device via a network), the data has to be send from the application layer and needs to descend dwon to the physical layer of that device and ascend from the pysical layer to the application layer on the client device
- Client / Server Architecture (each performs different tasks)
	- Server
		- listens on a certain IP and portnumber for clients attempting to send a request to connect to the server via the portnumber
	- Client
		- can send requests to connect to a certain Ip and portnumber

Sender
1. Application
	- where we write out c++ code
2. Presentation
	- in order to get data to another destination it has to be broken down into packets
	- Serilization, buffering
3. Session
	- send it
	- Sockets
4. Transport
	- how packets and bytes will be transported
	- tcp and udp protokols
5. Network
	- Ip (internet protokol) address of the machine
	- identifying each machine on the network
6. Data Link
	- networking card, ethernetport, macaddress
7. Physical
	- electronics and hardware, connections between machine and router

Router
1. Physical -> 2. DataLink -> Network -> Network -> DataLink -> Physical

Receiver
Ascend all 7 Layers

## IP Addresses
- each machine on a network has it's own unique Ip address
- Loopback address 127.0.0.1
- used to test both client and server on a signle maching during dev

## Ports
- every machine that has a IP address has ports available on that machine for which that machine can send and receive data
- port number (16 bit address) can be any integer between 1 and 65.535
- ports 1 - 1023 are described as well known ports and are reserved for specific applications (port 21 FTP)
- it is recommended to choose a number over 1024 to be sure that it isn't already in use

## Sockets
- A pipe between two computers on a network through which data flows
- binding IP Address and Portnumber to a dedicated socket
	- one for the server
		- which listens out for clients to connect that socket
	- one for the client
		- which will specify the details of the server (IP adress and portnumber)
- two different types of sockets
	- streaming socket utilises TCP (transmission control protocol)
		- needs a connection
		- more secure connection between server and client
		- reliability
	- Datagram socket UDP (user datagram protocol)
		- does not require connection to be established
		- less reliable and less secure but faster

1. Create a Socket
- server creates a new socket
	- it need to be bound to an IP or port number
	- Status is unbound 
2. Binding A Socket
- Bind the server to a valid IP address and portnumber
- Statis is bound
3. Listening
- Now we can listen for incoming reuest on those details (IP address and portnumber), for clients to connect to the socket through that portnumber

4. Client Socket
- Creates its own socket (unbound)
	- Staut unbound
- Bound (IP address the same if it's the same computer)
- Portnumber will be different
	- Status Bound
- once it#s bound it can put out a request to connect on the portnumber of the server
- once the request has been acknowledged and accepted by the server the server duplciates the socket to enable the connection between the server and the client
	- Status connected
	- but at the same time the original socket is maintaied that will contine to listen for other connections on that port

## Server Functions
	
2. Create a socket - socket()
	-
3. Bind the socket - bind()
	-
4. Listen on the socket - listen()
	-
5. Accept a connection - accept(), connect()
	-
6. Send and receive data - recv(), send(), !!!NOT ALLOWED!!! recvfrom(), sendto()
- send
	-
- recv
	-
7. Disconnect - closes()

## Client Functions

2. Create a socket - socket()
	- 
3. connected to the server - connect()
	- client connects to the server by assigning the servers IP address and port to a sockaddr structure and passing this along with the client socket to the connect function
4. Send and receive data - recv(), send(), !!!NOT ALLOWED!!! recvfrom(), sendto()
- send
	-
- recv
	-
5. Disconnect - closes()
