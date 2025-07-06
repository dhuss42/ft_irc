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

## IRC
- Internet Relay Chat (IRC)
	- text-based communication protocol on the Internet
	- real-time messaging (private and public)
	- user can exchange direct messages and join group channels
	- A typical setup involves a single process (the server) forming a central point for clients (or other servers) to connect to performing the required message delivery/multiplexing and other functions.

### IRC server
- IRC server
	- IRC servers are connected togther to form a network
	- handles clients connected to it
	- relay messages between users and servers
	- maintain:
		- user sessions
		- channel memberships
		- permissions and modes
	- only component of the protocol which is able to link all the other components together
- Reference
	https://www.rfc-editor.org/rfc/rfc2813#section-10

- IRC Protocol Services (Servers)
	- Client Locator
		- for messag exchange clients musst be able to locate each other
		- clients register using a label
			- is used by servers and client to know where that client is
		- servers keep track of all the labels
	- Message Relaying
	- Channel Hosting and Management
		- named group of users that receive all messages in that channel
		- Servers host channels and provide message multiplexing
		- Servers keep track of channel members

- Global database
	- every server maintains a global state database about the whole IRC network
	- Servers
		- identified by their name max len 63 chars
	- Clients
		- for ever client servers musst have:
			- a netwide unique identifier
			- the server to which the client is connected
	- User
		- distinguished from other users by nickname (max len 9 chars)
		- server must have info about all users:
			- nickname
			- name of the host that the user is running on
			- the username of that user on that host
			- server to which the client is connected
	- Services
		- distinguished from other services by a service name (a nickname and server name)
			- nickname has max len 9 chars
		- servers musst know the service type
		- services have different acces to server than users
			- 2.2.2
	- Channels
		- are not known to all servers
		- when channel existance is known to a server, it musst keep track of the members and channel modes

- Server Specification
	- Character codes
		- the characters {}|^ are lower case equivalents of the characters []\~
	- Messages
		- Most communication between servers do not generate any reply, as servers mostly perform routing tasks for the clients.

- Messages
	- the server parses the complete message and returns errors if there are any
		- if it encounters a fatal error while parsing a message
			- error must be sent to client and parsing terminated
		- fatal errors
			- incorrect command
			- unknown destination to the server
			-  not enough parameters
			- incorrect privileges
		- if full set of parameters
			- each must be checked and appropriate responses sent to the client
	- Join message (4.2.1)
		- used by client to start listening a specific channel
		- checked by local server if client is permitted to join
	- Mode message (4.2.3)
		- allows both users and channels to have their mode changed
		- when parsing these it is recommended to parse the enitre message, and then pass on the changes
	
- Connection 'Liveness'
	- server polls each of its connections
		- PING command is used if the server doesn't get a response from its peer in a given amoun of time
		- if connection doesn't respond in time its connection is closed

- Accepting a client to server connection
	- Users (5.2.1)
		- When server registers a new user connect it sends:
			- user identifiers upon which it was registered
			- the server name and version
			the server birth information
			- available user and channel modes
			- introductory message
			- other things I don't quite understand
	- Services (5.2.2)
		- when new service connection is made same requirements as for user count
		- different replies are sent

- Terminating server-client connections
	- When client connection closes a QUITE message is generated on behalf of the cliebt by the server. No other message is generated or used.

- Track nickname changes (5.6)
	- servers keep history of nickname changes
	- Messages which must trace nick changes are
		- KILL (the nick being dosconnected)
		- MODE
		- KICK (the nick being removes from channel)
	- server first checks for existance of nickname
	- then check its history to see who that nick now belongs to

- Tracking recently used nicknames (5.7)
	- Nickname Delay
		- servers keep track of nicknames which were recently used and were relesed as the result of a KILL message. 
		- Those nicknames are then unavailable to the server clients and cannot be re-used (even though not used at that time) for a certain period of time

- Flood control of clients 5.8
	- algorithm that avoids spaming of clients and services

### IRC client
- IRC client
	- connect to IRC servers in order to join channels
	- A client is anything connecting to a server that is not another server
	- User Clients
		-  programs providing a text based interface that is used to communicate interactively via IRC
			- Operators
				- to keep order within IRC network
					- basic network tasks, disconnecting and reconnecting servers as needed
	- Service Clients
		- not used manually or for talking
		- limited access to chat functions of the protocol
		- more access to private data from server
		- provide some kind of service to users like collecting statistics about the origin of users conencted on the IRC network
	- Channels
		- names begin with &, #, +, !
			- define chanel types
			- irrelevant for client-server protocol
		- length up 50 chars
			- no spaces
			- no comma
			- no ctrl g or ASCII 7
		- channel names are case insensitive

- Client to server connections
	- Character codes
		- message can be up to 512 bytes total including the delimiters
			- max of 510 characters
		- messages might arrive split or together because TCP does not preserve message boundaries
		- IRC uses control codes the sequence \r\n → carriage return + line feed to delimit messages.
		- parse the incoming stream by looking for the delimiters
	- Messages (PARSING OF COMMANDS 2.3)
		- consist of three main parts all separated by a space
			- prefix (optional)
				- :<prefix>
				- indicates origin 
				- usually only sent by servers
				- users should only use it with their nickname
			- command
				- either a named command JOIN, PRIVMSG or a three digit numeric code like 001 etc
			- command parameters (max 15)
	- Message format
		- empty message are silently ignored
	- Numeric replies (2.4)
		- messages sent to server usually generate reply
		- most common reply is numeric
			- used for errors and normal replies
			- one message consisting of
				- sender prefix
				- three digit numeric
				- target of the reply
			- same as message just that keyword is numeric
			- list of replies (5.)
- Message Details (3) RELEVANT FOR PARSER
	- ERR_NO_SUCHSERVER
		- target of message could not be found
		- server musst not send any other replies any other replies after this error
		- server parses the complete message of the client and returns appropriate errors
	- Channel operator Commands that are mandatory
		- KICK
			- Eject a client from the channel
		- INVITE
			- Invite a client to a channel
		- TOPIC
			- Change r view channel topic
		- MODE
			- Change the channel's mode
				- i set/remove Invite-only channel
				- t set/remove the restrictions of the TOPIC command to channel operators
				- k set/remomve the channel key(password)
				- o give/take channel operator privilege
				- l set/remove the user limit to channel
- optional features for messages (4)
- Reply messages (5)

### IRC Channels
- characterised by
	- name
		- up to 50 chars
		- case insensitve
		- names begin with &, #, +, ! (prefix)
			- for different channel types
		- no spaces
		- no comma
		- no ctrl g or ASCII 7
		- colon is used as delimiter for channel mask
	- properties
	- current members

- Prefixes
	- & prefix
		- local to the server where they are created
	- + prefix
		- channel does not support channel modes
		- all modes are unset except the t channel flag
	- ! prefix
		- user that creates channel with ! is the channel creator
		- user is automatically operator
		- creator can change modes of channel with operators cannot

- Channel properties
	- each channel has it's own properties
	- defined by channel modes
		- can be manipulated by channel members
		- modes affect how the server manages the channels

- Priviledged Channel Members
	- are allowed to perform these actions on channel
		- INVITE 
			- invites a client to an invite-only channel (mode +i)
		- KICK
			- Eject a client from the channel
		- Mode
			- Change the channel's mode, as well as members' privilieges
		- PRIVMSG
			- sending messages to the channel (mode +n, +m, +v)
		- TOPIC
			- change the channel topic in a mode +t channel

- Channel Operators
	- are considered to 'own' the channel
		- shared among channel operators
	- identified with @ next to nickname

- Channel lifetime
	- standard channels (&, #, +)
		- created implicitly when the first user joins it
		- cease to exist when last user leaves
		- user who creates the channel is operator
			- except for +
	- safe channels (!)
		- not implicitly created
		- users send a special join command to the channel
			- User only chooses part of the channel name
				- server automaticlly prepends the user provided name with a channel identifier of 5 chars
		- user automatically becomes channel creator
		- server does not allow the creation of another channel with the same short name

- Channel Modes (4.) (I only added the ones from the subject)
	- i
		- toggle invite-only channel flag
		- only by channel operator, or if their mask matches Invite-list
		- restricts usage of invite command
	- t
		- toggle the topic settable by channel operator only flag
	- k
		- set/remove channel key(password)
		- channel key musst be made visible to the channel members in the reply sent by the server to a Mode query
	- o
		- give/ take channel operator priviledge
	- l
		- toggle userlimit
		- when limit is reached server forbds local users to join the channel
		- value of limit is only available to the channel members in the repl sent by the server to a Mode query

- Tracking recently used channels (Channel Delay)
	- applies to channels with # prefix
	- if all channel members leave the channel, the channel becomes unavailable
		- becomes available when a user joins or
		- delay period has expiered
			- channel ceases to exit and may be re-created
			- duration should consider size user-wise and usual duration of networksplits

- Safe Channels
	- prefixed with !
	- Channel Identifier
		- current time in seconds since epoch is converted in a string of five characters using this base:
			- "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"
			- each character has a value starting from 0 for 'A' to 35 for '0'
			- divide seconds by 36 and take the character (do it five times)
			- last value will be the first, if there less than 5 numbers than it's padded with an A
	- do not become unavailable
	- i think the still have a time to live

- Name Space
	- server must forbid the creation of a new channel which has the same shortname of a channel currently existing

### IRC Architecture
- Architecture
	- IRC network is a group of severs connected to each other
	- A Single server is the simplest IRC network
	- IRC protocol allows only for communication via the server not for client to client direct communication

- IRC concepts
	- 1 to 1 communication
		- is directly between the clients and the intermediate server, no other entity can see the message
	- 1 to many
		- to a channel
		- to A host/server mask
		- to a list of destinations
	- 1 to all
		- client to server
		- server to server


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

- multiplexing
	- at the moment I think it refers to a single message from one user being spread to all users in the same channel

- TCP

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
				sa_family_t	sin_family; /* address family: AF_INET */
				in_port_t	  sin_port;   /* port in network byte order */
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
	-  The getprotobyname() function shall search the database from the beginning and find the first entry for which the protocol name specified by name matches the p_name member, opening and closing a connection to the database as necessary.
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
		int			  ai_flags;
		int			  ai_family;
		int			  ai_socktype;
		int			  ai_protocol;
		socklen_t		ai_addrlen;
		struct sockaddr *ai_addr;
		char			*ai_canonname;
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
	- linux manual page says to avoid the use of signal and use sication instead
		- it's behaviour varies accross UNIX versions and has also varied historically across different versions of Linuxs
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
	- int sigaction(int signum, const struct sigaction *_Nullable restrict act, struct sigaction *_Nullable restrict oldact);
		- signum
			- signum specifies the signal and can be any valid signal except SIGKILL and SIGSTOP.
		- act
			- If act is non-NULL, the new action for signal signum is installed from act
		- oldact
			- If oldact is non-NULL, the previous action is saved in oldact
	- struct sigaction {
               void     (*sa_handler)(int);
               void     (*sa_sigaction)(int, siginfo_t *, void *);
               sigset_t   sa_mask;
               int        sa_flags;
               void     (*sa_restorer)(void);
           };
		- sa_handler - the signal handler
			- SIG_DFL - set to default signal handler
			- SIG_IGN - the signal is ignored
			- pointer to a signal handling function void fun(int sig);

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
		dev_t	 st_dev;	 /* ID of device containing file */
		ino_t	 st_ino;	 /* inode number */
		mode_t	st_mode;	/* protection */
		nlink_t   st_nlink;   /* number of hard links */
		uid_t	 st_uid;	 /* user ID of owner */
		gid_t	 st_gid;	 /* group ID of owner */
		dev_t	 st_rdev;	/* device ID (if special file) */
		off_t	 st_size;	/* total size, in bytes */
		blksize_t st_blksize; /* blocksize for file system I/O */
		blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
		time_t	st_atime;   /* time of last access */
		time_t	st_mtime;   /* time of last modification */
		time_t	st_ctime;   /* time of last status change */
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
			   int   fd;		 /* file descriptor */
			   short events;	 /* requested events */
			   short revents;	/* returned events */
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

## Order of Server Functions

1. Create a socket - socket()
2. Bind the socket - bind()
3. Listen on the socket - listen()
4. Accept a connection - accept()
5. Send and receive data - recv(), send()
6. Disconnect - closes()

## Order of Client Functions

1. Create a socket - socket()
2. connected to the server - connect()
3. Send and receive data - recv(), send()
4. Disconnect - closes()
