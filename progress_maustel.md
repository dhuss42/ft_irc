# maustel's file to track progress for irc and to list the next to dos

## to do
[x] run david's project and tests
[x] install irssi and play around
[] get into davids code
[] read about unknown concepts
[] read again subject
[x] try if push to repo works
[x] try out commands we have to implement with official server
[x] create git branch Milena

## ====== Day 1 == 08.09.2025 ======
- created new access token and did first push on github
- installed irssi (https://software.opensuse.org/download.html?project=home:ailin_nemui:irssi-an&package=irssi-an)
- first steps irssi: https://irssi.org/New-users/

## ====== Day 2 == 09.09.2025 ======
- run David's code
- Connection created between client and server with following commands:
	- /network add IRC_David
	- /server add -network IRC_David localhost 6667
	- /connect IRC_David

    or just: /connect localhost 6667 <password>

- Inform about sockets (https://www.youtube.com/watch?v=D26sUZ6DHNQ)
- Common socket operations:
    socket(): Create a new endpoint
    bind(): Assign an address to the socket
    listen(): Start accepting connections
    accept(): Accept incoming connections
    connect(): Connect to a remote socket
    send()/recv(): Exchange data
    close(): Close the connection

- run David's test
- read concepts.md

## ====== Day 3 == 10.09.2025 ======
- checking out channel commands required by the subject:
    ∗ KICK - Eject a client from the channel
    ∗ INVITE - Invite a client to a channel
    ∗ TOPIC - Change or view the channel topic (set: "/topic #Amselchat :amsamsmas" --- check: "/topic #Amselchat")
    ∗ MODE - Change the channel’s mode: ("/mode +i" -> set invite-only channel)
        · i: Set/remove Invite-only channel
        · t: Set/remove the restrictions of the TOPIC command to channel operators -> toggle the topic settable by channel operator only flag
        · k: Set/remove the channel key (password that has to be added to /join #channel password)
        · o: Give/take channel operator privilege (so they are able to kick another user from the channel for example)
        · l: Set/remove the limit of number of users to channel

- other commands requiered by the subject:
    - authenticate -> ?
    - set nickname -> NICK <nickname>
    - set username -> USER <user> <mode> <unused> <realname>
    - join a channel -> JOIN <channel>
    - send private messages -> PRIVMSG <user> :<message-content> --- but in irssi: "/msg <user> private <message-content>"

## ====== Day 4 == 11.09.2025 ======
- what is a service?
    Unlike users, service clients are not intended to be used manually
   nor for talking.  They have a more limited access to the chat
   functions of the protocol, while optionally having access to more
   private data from the servers.
   Services are typically automatons used to provide some kind of
   service (not necessarily related to IRC itself) to users.  An example
   is a service collecting statistics about the origin of users
   connected on the IRC network.

   - Read IRC Protocols

----------- call david ----------
- CAP LS 302 -> question from client to server: what can I do on this server?
    https://modern.ircdocs.horse/#connection-registration
- we need more commands as the subject requires, because of authetification
- Prio:
    CAP LS 302
    JOIN
    MSG

## ====== Day 5 == 12.09.2025 ======
- Read authentication file: https://modern.ircdocs.horse/#connection-registration

- in our IRC server the commands during registration are:
    ## --- initial connection phase ---
    - CAP LS 302 -> answer from server: list of available capabilities
    ## --- capability negotiation ---
    - JOIN : (client asks if this command exists)-> server acknowledges or rejects
    - CAP END
    ## --- authentication phase ---
    - PASS <password> -> server accepts or rejects
    ## --- registration phase ---
    - NICK <nickname> -> error if nickname in use, otherwise accept
    - USER <username> <hostname> <servername> :<realname> -> server: sends welcome message
    ## --- post registration phase ---
    - MODE <nickname> +i -> server: confirms mode change
    - WHOIS <nickname> (Requests information about users matching nickname) -> server: Send WHO replies
    ## --- keep alive ---
    - PING irc_custom -> server responds: PONG irc_custom

- commands to handle:
    ## --- server commands ---
    - CAP
    - PING
    - NOTICE
    - PASS
    - QUIT
    ## --- client commands ---
    - PRIVMSG -> (?) maybe channel command?
    - NICK
    - USER
    - WHOIS
    ## --- channel commands ---
    - JOIN
    - PART
    - KICK
    - MODE
    - TOPIC
    - INVITE

todo:
[] think about parsing structure
    - how to organize command and options?
    - start with simple commands and no structure
    - look up commands in manual
    - test commands in existing network with options
    - test commands in our server and check what happens so far

## ====== Day 6 == 13.09.2025 ======
- reading about good practice for irc perser
    https://modern.ircdocs.horse/impl.html

- decided how to structure parsing
- created class message
- started with parser
- idea:
    - split raw message into command and vector of parameters
    - while splitting check if there are any ' ' (not valid) or ':' (after that, rest of raw message is one parameter)
    - then go to specific handlers

- to do:
    - parsing function devided into steps and single functions for each step

## ====== Day 7 == 16.09.2025 ======
- Info: irssi takes commands without ':' and adds them automatically
    - example: /topic #<channel> :this is the topic of the channel
        ->in irssi I type: /topic #<channel> this is the topic of the channel
        ->in server arrives: /topic #<channel> :this is the topic of the channel
- also too many qhitespaces are eliminated by irssi

- finished split function
- problem: command is empty in USER and JOIN at registration phase -> solved

- starting with most important handlerfunctions to make it possible to connect

- to do:
    - make run the parser prototype (compare with pseidoparser and test)

    - in server:
        - getName
        - getPassword
    - (?) -> handler functions directly in server??
    - improve handler functions with exceptions instead of errormessages and mor functionalities
    - maybe write operator overload function << for message to print the whole message for debugging
## ==== QUESTIONS ====
- Parser job:
    - if command does not exist or not enough parameters -> is handled already by irssi
    - but commands we do not handle have to throw error message
    - follow rules of protocol (for example nickname has maximum 15 char(?))
    - forbidden channel names
    - check for options
    - handle / execute functions (that will be written by david)
    - if required, send response

- Parser = command parsing + command handlers?
- parser in ClientClass?

- do we have sendToCLient function?
    -> yes. But: not working here
    else if (message.find("CAP LS 302")  == 0)
	{
		sendMsg("irc_custom", "CAP * LS :");
	}

- at registration phase: MODE <nickname> +i -> at nickname the first letter isnt included
