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
        -> but on server it arrives as PRIVMSG

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
    PRIVMSG

## ====== Day 5 == 12.09.2025 ======
- Read authentication file: https://modern.ircdocs.horse/#connection-registration

- in our IRC server the commands during registration are:
    ## --- initial connection phase ---
    - CAP LS 302 -> answer from server: list of available capabilities
    ## --- capability negotiation ---
    - JOIN : (client asks if this command exists)-> server acknowledges or rejects
    - CAP END
    ## --- authentication phase ---
    - PASS <password> -> server accepts or rejects (accept: setflag=true) -> david ändert aurhent logik später
    ## --- registration phase ---
    - NICK <nickname> -> error if nickname in use, otherwise accept (accept: setflag=true)
    - USER <username> <hostname> <servername> :<realname> -> server: sends welcome message (is done in authentication function if all 3 flags are set)
    - https://datatracker.ietf.org/doc/html/rfc1459#section-4.1.3
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

- problem: parser prototype seems to not connect properly -> david found bug: in handlePass set _registered=true

- to do:
    - make run the parser prototype (compare with pseudoparser and test)

    - in server:
        - getName
        - getPassword
    - (?) -> handler functions directly in server??
    - improve handler functions with exceptions instead of errormessages and mor functionalities
    - maybe write operator overload function << for message to print the whole message for debugging

## ====== Day 8 == 17.09.2025 ======
- improving handler functions

- issue /msg : segfault in Channel::broadcast, std::cout << "[DEBUG] _users.size(): " << _users.size() << std::endl;

- how to handle errors and replies correctly?
-> :server 462 nickname :You may not reregister
    - :server - The server prefix (required)
    - 462 - The three-digit numeric code
    - nickname - The target nickname
    - :You may not reregister - The message text (colon-prefixed)
    -->sendMessage(servername, errorcode + nickname + ":" + errormessage)

- idea: make IRCError file and do enum class IRCErrorCode

## ====== Day 9 == 18.09.2025 ======
- made ircerror and irc response files and finished sendError and sendResponse

- call with david
    - get functions in client for password (handlePass) and server name (sendMsg) -> david
    - sendErrormessage in client --> ja
    - actually all handlers should be in server? -> message/parser class
    - CAP * LS -> Capabilities supported: -> do I list it in an extra sendMsg? -> später
    - PRIVMSG: segfault in Channel::broadcast, std::cout << "[DEBUG] _users.size(): " << _users.size() << std::endl;
        -> david (später)
    - Abgabe wann anpeilen? -> Mo entscheiden ob 4/5 oder 11/12


## ====== Day 10 == 19.09.2025 ======


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

[x] do we have sendToCLient function?
    -> yes. But: not working here
    else if (message.find("CAP  LS 302")  == 0)
	{
		sendMsg("irc_custom", "CAP * LS :");
	}
    -> yes it works, it gets translated as "Capabilities supported:"

[x] at registration phase: MODE <nickname> +i -> at nickname the first letter isnt included
[x] at registration phase: MODE <nickname> +i -> at nickname the first letter isnt included

[x] (???) my parser does not keep the conenction to server, why??
    - David solved it! in handlePass: this->_registered = true

[x] how to avoid git conflicts? -> https://stackoverflow.com/questions/70153247/branching-strategy-to-avoid-conflicts-on-two-branches
[x] how to avoid git conflicts? -> https://stackoverflow.com/questions/70153247/branching-strategy-to-avoid-conflicts-on-two-branches

//First, before starting the work, make sure you're starting from an up to date main branch like this:
- git checkout main
- git pull
- git checkout branchName
//Then do the changes on your branch called branchName, and make a commit there:
// Do your code changes
- git add .
- git commit -m "Fix Code"
//Let's say, meanwhile someone updated the main branch.
//So, you'll need to rebase your code before pushing.
//Do the following:
- git checkout main
- git pull
- git checkout branchName
- git rebase main
//Now you have up-to-date code on your branch as well.
//Feel free to push:
- git push origin branchName
//Now you can create a pull request and merge it to the main as usual.

----- questions for david ------
- look for transcendence partner?

----- questions for david ------
- look for transcendence partner?
