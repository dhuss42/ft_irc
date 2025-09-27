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
    [x] CAP LS 302
    [~] JOIN
    [] PRIVMSG

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
- reading about good practice for irc parser
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
- finished handleCap

- handleJoin:
    - I need getChannels(): Returns all channels as a vector
    - addUser should be boolean

- todo:
    - finish handleJoin
    - think about it if it makes sense
    - test it

## ====== Day 11 == 22.09.2025 ======
- ???: /join 0 -> The server will process this command as though the client had sent a PART command for each channel they are a member of.
    -> but irssi does not do so but opens a chatwindow called #0
    -> when does this happen? How can I avoid?

- removeUser(&_client) not sure if working -> david has to check it when join is done

- join: https://modern.ircdocs.horse/#join-message
    [x] client can join that channel? -> is done in Client::addUser
        - key ok? - ERR_BADCHANNELKEY
        - userlimit? - ERR_CHANNELISFULL
        - invite only channel? - ERR_INVITEONLYCHAN
    [not-working] /join 0 -> remove user from all channels -> irssi does not handle this, so dont we
    [to-do] handle more than 1 channel and key -> seprataed by ',' -> first test with official server
    [x] send messages to channel
        [x] when entering
        [x] when leaving
    -( receive messages from PART, KICK, MODE)
    [x] if join is successfull, server must send in this order:
        1) A JOIN message with the client as the message <source> and the channel
            they have joined as the first parameter of the message
            ( <nickname> [~maustel@81.56.177.83] has joined <channelname>)
        [to-test] 2) The channel’s topic (with RPL_TOPIC (332) and optionally RPL_TOPICWHOTIME (333)),
            and no message if the channel does not have a topic.
        3) A list of users currently joined to the channel (with one or more RPL_NAMREPLY (353)
            numerics followed by a single RPL_ENDOFNAMES (366) numeric).
            These RPL_NAMREPLY messages sent by the server MUST include the requesting client
            that has just joined the channel.

    - Problems:
        [x] sendResponse and sendMsg will be printed in main window if irssi not in the channel window
            -> need prefix ("name" in sendMsg) like
            std::string prefix = _client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname() + " PRIVMSG " + channel->(getName) + " :";

- todo
    - handleJoin:
        - do extra function for sending messages to channel to make it more readable
        - do loop to be able to handle more than one joinchannel
    [x] handleMsg:
        - read about it https://modern.ircdocs.horse/#privmsg-message

## ====== Day 12 == 23.09.2025 ======
- handleMsg: PRIVMSG <target> :<texr>
    - target can be
        - user
            - If <target> is a user and that user has been set as away, the server may reply with
                an RPL_AWAY (301) numeric and the command will continue.
            [x] no such nick:  ERR_NOSUCHNICK (401) "<target>: No such nick/channel"
        - channel
            [x] If a message cannot be delivered to a channel, the server SHOULD respond with an ERR_CANNOTSENDTOCHAN (404)
                -> the user has to have joined the channel before
            [x] no such channel:  ERR_NOSUCHNICK (401) "<target>: No such nick/channel"
    [?] do we need to handle $ ?

    [x] needed from David:
        - Client* Server::getClient(std::string name)

    [x] if client not exists, irssi still opens new window, but also in existing servers
    -> just channel sends message not in new window


- MODE: -> special parsing would be useful (?)
    [x] If <target> is a channel that does not exist on the network, the ERR_NOSUCHCHANNEL (403) numeric is returned
    [x] If <modestring> is not given, the RPL_CHANNELMODEIS (324) numeric is returned -> message with the currently set modes
    [x] check if user has channel operator priviliges, if not ERR_CHANOPRIVSNEEDED (482)
    - if no '+' or '-' -> will be counted as '+'
    - must have parameter (check for valid parameter):
        - k -> channel key (without parameter will be ignored, with paramater will take the first word found)
            - (?) check for valid key? (for example invalid characters?)
        - o -> operator privilege (Users with this mode may perform channel moderation task) -> user mode
            - if not enough paramters, send error: MODE Not enough paramters
        - l -> user limit of channel
            - if not enough paramters, send error: MODE Not enough paramters
    - must not have parameter:
        - i -> invite only channel
        - t -> restrictions of topic command to channel operators
    - handle more than one mode change (also "+it" is possible instead of +i +t.
        if mode needs parameter, the first parameter will be the one for the first modechange)
        +k it -> sets passkey to "it"
        +ki t -> sets passkey to t and sets also inviteOnly
    - when changed mode, response  mode/#channel [+ki] by nickname to all members of the channel
    - unknown mode character -> sendError
    - think also of user mode -i at registration phase

- mode parsing:
    - /mode #channel modestring -> params[mode, channel, modestring]
    - modestring: everything after target
    - or just build string of params[2] until params[end]

## ====== Day 13 == 25.09.2025 ======
- new logic for mode

---- call david -----
- irc handles names case insensitive -> be aware if handled everywhere or segfault
- authentification has to be modified: check in pass, user if error -> disconnect subito
    -> as well in nick, but unly first time nick is called
- trancendence: I sent message on slack, didnt find other people searching

- finished mode but needs a lot of debugging

## ====== Day 14 == 26.09.2025 ======
- lots of debuggung
- found iterating error
- found out to not use in if concdition: Message.params[i].empty() but i < message.params.size()
    because Message.params[i].empty() has undefined behaviour if it goes beyond borders

[???] bug if /mode #channel +o channel -> Error message ok, but channel will be closed
    -> added a " " for now

- todo:
    [x] rename HandlerHelpers -> ModeHelperFcts
        - or add ModeHelpers and copy from HandlerHelpers so I can use HandlerHelpers for other functions
    - Mode main function more clean
    - mode functions with commands
    [x] mode tests with error messages from david
    - join main function more clean and with more than one command
    - join functions with commands
    [x] rename ParseHandler -> Parser

    [x] /join inv only channel -> after error message trotzdem irssi window
        -> must be client->sendError(serverName, IrcErrorCode::ERR_INVITEONLYCHAN, this->name);
    [x] negative user still possible

## ====== Day 15 == 27.09.2025 ======


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

- ???: /join 0 -> The server will process this command as though the client had sent a PART command for each channel they are a member of.
    -> but irssi does not do so but opens a chatwindow called #0
    -> when does this happen? How can I avoid?

- sendMsg and response/error should actually be server functions or call it receiveMsg
    -> no its ok like that

----- questions for david ------

## General Info
- rebase vs merge
    When you rebase your branch onto their branch, you tell Git to make it look as though you checked out their branch cleanly, then did all your work starting from there. That makes a clean, conceptually simple package of changes that someone can review. You can repeat this process again when there are new changes on their branch, and you will always end up with a clean set of changes "on the tip" of their branch.
    When you merge their branch into your branch, you tie the two branch histories together at this point. If you do this again later with more changes, you begin to create an interleaved thread of histories: some of their changes, some of my changes, some of their changes. Some people find this messy or undesirable.
