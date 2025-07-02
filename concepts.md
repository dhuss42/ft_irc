# IRC structure

## Server, networking, client connection management
1. main
- validate command-line args
- instantiate server object
- run server
2. Server
- listening on socket, accept connections
- store connected cliebt objects
- maintain list of active channel objects
- poll() Loop
3. Client
- connected IRC client
- track registration state, nickname, username, etc

## Parsing and command handling
4. Channel
- users in a channel
- track channel modes, operations, topic, etc
5. Command Parser
- aggregate partial data into full IRC lines
- split lines into command and parameters
6. Command Handlers
- Registration commands> PASS, NICK, USER
- Channel management: JOIN, PART, KICK, INVITE, TOPIC, MODE
- Messaging> PRIVMSG, NOTICE
- send appropriate server replies
7. utils
- string manipulation
- error handling
- conversions

# new concepts

- Internet Relay Chat (IRC)
	-  text-based communication protocol on the Internet
	- real-time messaging (private and public)
	- user can exchange direct messages and join group channels
- IRC server
	- IRC servers are connected togther to form a network
	- handles clients connected to it
	- relay messages between users and servers
	- maintain:
		- user sessions
		- channel memberships
		- permissions and modes
- IRC client
	- connect to IRC servers in order to join channels
- solid standard protocols
	- IRC communication follows standardized protocols
		- RFC 1459 (original IRC specification).
		- RFC 2810–2813 (updated IRC protocol specifications).
	- defines
		- message formats
		- commands
		- numeric replies
		- error handling


- I/O operations must be non-blocking
	- blocking functions wait on certain conditions until they execute fully
	- non-blocking returns immediately
		- allows programm to coninue doing other work
		- important for servers
			- don't want to freez waiting for one of them to send data
			- possible to monitor many sockets at once
			- React when any of them becomes ready for reading or writing
		- poll(), epoll()
- IRC client as reference
	- HexChat
	- Irssi
	- WeeChat
- operators in IRC
	- IRC operators are privileged users with special permissions.
	- Server authenticates operators, usually via password.
	- Operators are essential for moderating and maintaining IRC networks.



# new external functions

## Server and client Functions
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
	- client connects to the server by assigning the servers IP address and port to a sockaddr structure and passing this along with the client socket to the connect function
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
	- int getsocknam(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen)
	- returns the current address to which the socket sockfd is bound, in the buffer pointed to by addr.
		addrlen indicates the amount of space pointed to bz addr. On return it contains the actual size of the socket address.
	- returns
		- -1 on error
		- 0 on success

- getprotobyname
	-  #include <netdb.h>
	- struct protoent *getprotobyname(const char *name);
	-  The getprotobyname() function shall search the database from the
	beginning and find the first entry for which the protocol name
	specified by name matches the p_name member, opening and closing a
	connection to the database as necessary.
	- returns a pointer to a protoent structure if the
       requested entry was found, and a null pointer if the end of the
       database was reached or the requested entry was not found.
       Otherwise, a null pointer is returned.

- gethostbyname
	- The gethostbyname() function returns a structure of type hostent
       for the given host name.  Here name is either a hostname or an
       IPv4 address in standard dot notation (as for inet_addr(3)).  If
       name is an IPv4 address, no lookup is performed and
       gethostbyname() simply copies name into the h_name field and its
       struct in_addr equivalent into the h_addr_list[0] field of the
       returned hostent structure.  If name doesn't end in a dot and the
       environment variable HOSTALIASES is set, the alias file pointed to
       by HOSTALIASES will first be searched for name (see hostname(7)
       for the file format).  The current domain and its parents are
       searched unless name ends in a dot.
	- return the hostent structure or a null pointer if an error occurs.

- getaddrinfo
	- #include <sys/types.h>
	- int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
	- getaddrinfo() returns one or more addrinfo structures, each of which contains an Internet address

	The addrinfo structure used by getaddrinfo() contains the following fields:

	struct addrinfo {
		int              ai_flags;
		int              ai_family;
		int              ai_socktype;
		int              ai_protocol;
		socklen_t        ai_addrlen;
		struct sockaddr *ai_addr;
		char            *ai_canonname;
		struct addrinfo *ai_next;
	};


- freeaddrinfo
	- void freeaddrinfo(struct addrinfo *res);
	- The freeaddrinfo() function frees the memory that was allocated for the dynamically allocated linked list res.

## conversion
see [Endianness](## Endianness)
- htons
	- Host To Network Short” → converts a 16-bit (short) number (like the port) to network byte order (big endian).
	- #include <arpa/inet.h>
	- converts the unsigned short integer hostshort from host byte order to network byte order.
	- pass portnumber and returns value in TCP/IP network byte order -> for the network to understand
	- have to pass port number to this function and store the retuns value in sockaddr_in struct .sin_port
- htonl
	- “Host To Network Long” → converts a 32-bit (long) number (like an IPv4 address) to network byte order.
	-  converts the unsigned integer hostlong from host byte order to network byte order.
- ntohs
	- converts the unsigned short integer netshort from network byte order to host byte order.
- ntohl
	- converts the unsigned integer netlong from network byte order to host byte order.
- inet_addr
	- Converts the IPv4 address string "127.0.0.1" directly into the required 32-bit network byte order integer.
	- converts the Internet host address cp from IPv4 numbers-and-dots notation into binary data in network byte order.
	- If the input is invalid, INADDR_NONE (usually -1) is returned. Use of this function is problematic because -1 is a valid address (255.255.255.255). Avoid its use in favor of getaddrinfo(3) which provide a cleaner way to indicate error return.
- inet_ntoa
	- converts the Internet host address in, given in network byte order, to a string in IPv4 dotted-decimal notation.
	- The string is returned in a statically allocated buffer, which subsequent calls will overwrite.

## signals
- signal
	- <signal.h>
	- void (*signal( int sig, void (*handler) (int))) (int);
	- sig	-	the signal to set the signal handler to. It can be an implementation-defined value or one of the following values:
		- SIGABRT
		- SIGFPE
		- SIGILL
		- SIGINT
		- SIGSEGV
		- SIGTERM
	- handler - the signal handler
		- SIG_DFL - set to default signal handler
		- SIG_IGN - the signal is ignored
		- pointer to a function - void fun(int sig);
	- returns
		- Previous signal handler on success or
		- SIG_ERR on failure

- sigaction
	- used to change the action taken by a process on receipt of a specific signal.

## file descriptor handling
- lseek
	- move to different location inside a file (good when file is fixed in size)
	- off_t lseek(int fd, off_t offset, int whence);
		- offset
			- the amount we want to offset from the starting location
		- whence
			- where we are offseting from
				- SEEK_SET: start at offset defined bytes
				- SEEK_CUR: offset from current location
				- SEEK_END: ofsset from end
	- returns
		- on success amount of offset that occured
		- on error -1
- fstat
	- #include <sys/types.h>
	- int fstat(int fd, struct stat *buf);
	- returns information about a file. No permissions are required on the file itself
	- stats the file pointed to by path and fills in buf.
	- the file to be stat-ed is specified by the file descriptor fd.
	- struct stat {
		dev_t     st_dev;     /* ID of device containing file */
		ino_t     st_ino;     /* inode number */
		mode_t    st_mode;    /* protection */
		nlink_t   st_nlink;   /* number of hard links */
		uid_t     st_uid;     /* user ID of owner */
		gid_t     st_gid;     /* group ID of owner */
		dev_t     st_rdev;    /* device ID (if special file) */
		off_t     st_size;    /* total size, in bytes */
		blksize_t st_blksize; /* blocksize for file system I/O */
		blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
		time_t    st_atime;   /* time of last access */
		time_t    st_mtime;   /* time of last modification */
		time_t    st_ctime;   /* time of last status change */
		};
	- returns
		- 0 on success
		- -1 on error

- fcntl only as follows: fcntl(fd, F_SETFL, O_NONBLOCK);
	- #include <fcntl.h>
	-  manipulate file descriptor
	-  F_SETFD (int)
		- Set the file descriptor flags to the value specified by arg.
	- O_NONBLOCK set it to onon blocking
		- for example accept() will not block the whole server if no client is connecting
	- returns
		- other than -1 on success
		- -1 on error

- poll (or equivalent) (select, kqueue, epoll)
	- wait on multiple file descriptors to be ready for input and output with one single call
	- handle them dynamically while the come in
	- poll()
	- #include <poll.h>
	- int poll(struct pollfd *fds, nfds_t nfds, int timeout);
		- fds is an array of structures of the following form
			struct pollfd {
               int   fd;         /* file descriptor */
               short events;     /* requested events */
               short revents;    /* returned events */
           };
			- fd: contains a file descriptor for an open file
			- events: input paramater, a bit mask specifying the events the application is interested in for the fd
			- revents: output parameter, filled by the kernel with the events that actually occured
		- nfds is the number of items inside the fds array
		- timeout: miliseconds that poll should block waiting for a file desriptor to become ready
			- negative value means an infinie timeout
			- 0 causes poll() to returns immediately
	- returns
		- a non negative value which is the number of elements in fds whose revents fields have been set to a nonzero value
		- system timeout before any filedescriptors became ready
		- -1 on error


# relevant concepts

## Endianness
- sequence in which bytes are arranged to represent dara
- Two common orders:
	- Big-endian: Most significant byte stored first (at lowest address)
	- Little-endian: Least significant byte stored first
- Host byte order
	- order used internally by computer's cpu architecture
	- can be big or little endian depending on computer
	- the same 32-bit int might be stored as bytes in a different order than on another computer
- Network byte order
	- standardized format used in networking to make sure all machines udnerstand the data consistantly
	- defined as big-endian
	- when computers send data across the network they musst convert their native host byte order to network byte order


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
5. Accept a connection - accept()
	-
6. Send and receive data - recv(), send()
- send
	-
- recv
	-
7. Disconnect - closes()

## Client Functions

2. Create a socket - socket()
3. connected to the server - connect()
4. Send and receive data - recv(), send()
- send
	-
- recv
	-
5. Disconnect - closes()
