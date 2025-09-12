# maustel's file to track progress for irc and to list the next to dos

## to do
[x] run david's project and tests
[x] install irssi and play around
[] get into davids code
[] read about unknown concepts
[] read again subject
[x] try if push to repo works
[x] try out commands we have to implement with official server
[] create git branch Milena

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

## ==== QUESTIONS ====
- David Irc server:
	- connecting works, but then further commands dont arrive, because
		it does not remain connected (?) Irssi: Not connected to server -> needs password at connection
	- why at connection from client to server it arrives the join command?

- Parser job:
    - if command does not exist or not enough parameters -> is handled already by irssi
    - follow rules of protocol (for example nickname has maximum 15 char(?))
    - check for options
    - handle / execute functions (that will be written by david)
    - if required, send response

- Parser = command parsing + command handlers?
- parser in ClientClass?
- how to handle send private messages -> PRIVMSG <user> :<message-content> --- but in irssi: "/msg <user> private <message-content>"

- authenticate -> ?


