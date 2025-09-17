/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcCodeGetters.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 16:21:34 by maustel           #+#    #+#             */
/*   Updated: 2025/09/17 16:21:34 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcErrors.hpp"
#include "IrcResponses.hpp"

// Helper function to get the string representation of error code
const char* getIrcErrorCodeString(IrcResponseCode code)
{
    switch (code)
	{
		case IrcErrorCode::ERR_NOSUCHNICK: return "401";			// No such nick/channel
		case IrcErrorCode::ERR_NOSUCHSERVER: return "402";		// No such server
		case IrcErrorCode::ERR_NOSUCHCHANNEL: return "403";		// No such channel
		case IrcErrorCode::ERR_CANNOTSENDTOCHAN: return "404";	// Cannot send to channel
		case IrcErrorCode::ERR_TOOMANYCHANNELS: return "405";	// Too many channels
		case IrcErrorCode::ERR_WASNOSUCHNICK: return "406";		// There was no such nick
		case IrcErrorCode::ERR_TOOMANYTARGETS: return "407";		// Too many targets
		case IrcErrorCode::ERR_NOORIGIN: return "409";			// No origin specified
		case IrcErrorCode::ERR_INVALIDCAPCMD: return "410";		// Invalid or missing CAP subcommand
		case IrcErrorCode::ERR_NORECIPIENT: return "411";		// No recipient given
		case IrcErrorCode::ERR_NOTEXTTOSEND: return "412";		// No text to send
		case IrcErrorCode::ERR_NOTOPLEVEL: return "413";			// No top level domain
		case IrcErrorCode::ERR_WILDTOPLEVEL: return "414";		// Wild card top level domain
		case IrcErrorCode::ERR_BADMASK: return "415";			// Invalid mask
		case IrcErrorCode::ERR_UNKNOWNCOMMAND: return "421";		// Unknown command
		case IrcErrorCode::ERR_NOMOTD: return "422";				// MOTD file missing
		case IrcErrorCode::ERR_NOADMININFO: return "423";		// No admin information available
		case IrcErrorCode::ERR_FILEERROR: return "424";			// File error
		case IrcErrorCode::ERR_NONICKNAMEGIVEN: return "431";	// No nickname given
		case IrcErrorCode::ERR_ERRONEUSNICKNAME: return "432";	// Erroneous nickname
		case IrcErrorCode::ERR_NICKNAMEINUSE: return "433";		// Nickname is already in use
		case IrcErrorCode::ERR_NICKCOLLISION: return "436";		// Nickname collision
		case IrcErrorCode::ERR_UNAVAILRESOURCE: return "437";	// Nick/channel is temporarily unavailable
		case IrcErrorCode::ERR_USERNOTINCHANNEL: return "441";	// They aren't on that channel
		case IrcErrorCode::ERR_NOTONCHANNEL: return "442";		// You're not on that channel
		case IrcErrorCode::ERR_USERONCHANNEL: return "443";		// User is already on channel
		case IrcErrorCode::ERR_NOLOGIN: return "444";			// User not logged in
		case IrcErrorCode::ERR_SUMMONDISABLED: return "445";		// SUMMON command disabled
		case IrcErrorCode::ERR_USERSDISABLED: return "446";		// USERS command disabled
		case IrcErrorCode::ERR_NOTREGISTERED: return "451";		// You have not registered
		case IrcErrorCode::ERR_NEEDMOREPARAMS: return "461";		// Not enough parameters --
		case IrcErrorCode::ERR_ALREADYREGISTRED: return "462";	// Already registered
		case IrcErrorCode::ERR_NOPERMFORHOST: return "463";		// No permission for operation
		case IrcErrorCode::ERR_PASSWDMISMATCH: return "464";		// Password mismatch
		case IrcErrorCode::ERR_YOUREBANNEDCREEP: return "465";	// You're banned
		case IrcErrorCode::ERR_KEYSET: return "467";				// Channel key already set
		case IrcErrorCode::ERR_CHANNELISFULL: return "471";		// Channel is full
		case IrcErrorCode::ERR_UNKNOWNMODE: return "472";		// Unknown mode
		case IrcErrorCode::ERR_INVITEONLYCHAN: return "473";		// Invite-only channel
		case IrcErrorCode::ERR_BANNEDFROMCHAN: return "474";		// Banned from channel
		case IrcErrorCode::ERR_BADCHANNELKEY: return "475";		// Bad channel key
		case IrcErrorCode::ERR_BADCHANMASK: return "476";		// Bad channel mask
		case IrcErrorCode::ERR_NOCHANMODES: return "477";		// Channel modes can't be changed
		case IrcErrorCode::ERR_BANLISTFULL: return "478";		// Ban list is full
		case IrcErrorCode::ERR_NOPRIVILEGES: return "481";		// No privileges
		case IrcErrorCode::ERR_CHANOPRIVSNEEDED: return "482";	// You're not channel operator
		case IrcErrorCode::ERR_CANTKILLSERVER: return "483";		// Can't kill server
		case IrcErrorCode::ERR_RESTRICTED: return "484";			// Restricted command
		case IrcErrorCode::ERR_UNIQOPPRIVSNEEDED: return "485";	// You need uniqop privileges
		case IrcErrorCode::ERR_NOOPERHOST: return "491";			// No O-line for your host
		case IrcErrorCode::ERR_UMODEUNKNOWNFLAG: return "501";	// Unknown MODE flag
		case IrcErrorCode::ERR_USERSDONTMATCH: return "502";		// Cannot change or set mode for other
		default: return "999"; 										// Unknown code
	}
}

// Helper function to get the string representation if reply code
const char* getIrcResponseCodeString(IrcResponseCode code) {
	switch (code)
	{
		case IrcResponseCode::RPL_WELCOME: return "001";
		case IrcResponseCode::RPL_YOURHOST: return "002";
		case IrcResponseCode::RPL_CREATED: return "003";
		case IrcResponseCode::RPL_MYINFO: return "004";
		case IrcResponseCode::RPL_ISUPPORT: return "005";

		case IrcResponseCode::RPL_BOUNCE: return "010";
		case IrcResponseCode::RPL_TRACELINK: return "200";
		case IrcResponseCode::RPL_TRACECONNECTING: return "201";
		case IrcResponseCode::RPL_TRACEHANDSHAKE: return "202";
		case IrcResponseCode::RPL_TRACEUNKNOWN: return "203";
		case IrcResponseCode::RPL_TRACEOPERATOR: return "204";
		case IrcResponseCode::RPL_TRACEUSER: return "205";
		case IrcResponseCode::RPL_TRACESERVER: return "206";
		case IrcResponseCode::RPL_TRACESERVICE: return "207";
		case IrcResponseCode::RPL_TRACENEWTYPE: return "208";
		case IrcResponseCode::RPL_TRACECLASS: return "209";

		case IrcResponseCode::RPL_STATSLINKINFO: return "211";
		case IrcResponseCode::RPL_STATSCOMMANDS: return "212";
		case IrcResponseCode::RPL_STATSCLINE: return "213";
		case IrcResponseCode::RPL_STATSNLINE: return "214";
		case IrcResponseCode::RPL_STATSILINE: return "215";
		case IrcResponseCode::RPL_STATSKLINE: return "216";
		case IrcResponseCode::RPL_STATSYLINE: return "218";
		case IrcResponseCode::RPL_ENDOFSTATS: return "219";

		case IrcResponseCode::RPL_UMODEIS: return "221";
		case IrcResponseCode::RPL_SERVLIST: return "234";
		case IrcResponseCode::RPL_SERVLISTEND: return "235";

		case IrcResponseCode::RPL_STATSLLINE: return "241";
		case IrcResponseCode::RPL_STATSUPTIME: return "242";
		case IrcResponseCode::RPL_STATSOLINE: return "243";
		case IrcResponseCode::RPL_STATSHLINE: return "244";
		case IrcResponseCode::RPL_STATSSLINE: return "245";
		case IrcResponseCode::RPL_STATSPING: return "246";
		case IrcResponseCode::RPL_STATSBLINE: return "247";

		case IrcResponseCode::RPL_LUSERCLIENT: return "251";
		case IrcResponseCode::RPL_LUSEROP: return "252";
		case IrcResponseCode::RPL_LUSERUNKNOWN: return "253";
		case IrcResponseCode::RPL_LUSERCHANNELS: return "254";
		case IrcResponseCode::RPL_LUSERME: return "255";

		case IrcResponseCode::RPL_ADMINME: return "256";
		case IrcResponseCode::RPL_ADMINLOC1: return "257";
		case IrcResponseCode::RPL_ADMINLOC2: return "258";
		case IrcResponseCode::RPL_ADMINEMAIL: return "259";

		case IrcResponseCode::RPL_TRACELOG: return "261";
		case IrcResponseCode::RPL_TRACEEND: return "262";
		case IrcResponseCode::RPL_TRYAGAIN: return "263";

		case IrcResponseCode::RPL_NONE: return "300";
		case IrcResponseCode::RPL_AWAY: return "301";
		case IrcResponseCode::RPL_USERHOST: return "302";
		case IrcResponseCode::RPL_ISON: return "303";
		case IrcResponseCode::RPL_UNAWAY: return "305";
		case IrcResponseCode::RPL_NOWAWAY: return "306";
		case IrcResponseCode::RPL_WHOISUSER: return "311";
		case IrcResponseCode::RPL_WHOISSERVER: return "312";
		case IrcResponseCode::RPL_WHOISOPERATOR: return "313";
		case IrcResponseCode::RPL_WHOWASUSER: return "314";
		case IrcResponseCode::RPL_ENDOFWHO: return "315";
		case IrcResponseCode::RPL_WHOISIDLE: return "317";
		case IrcResponseCode::RPL_ENDOFWHOIS: return "318";
		case IrcResponseCode::RPL_WHOISCHANNELS: return "319";

		case IrcResponseCode::RPL_LISTSTART: return "321";
		case IrcResponseCode::RPL_LIST: return "322";
		case IrcResponseCode::RPL_LISTEND: return "323";
		case IrcResponseCode::RPL_CHANNELMODEIS: return "324";
		case IrcResponseCode::RPL_UNIQOPIS: return "325";
		case IrcResponseCode::RPL_NOTOPIC: return "331";
		case IrcResponseCode::RPL_TOPIC: return "332";
		case IrcResponseCode::RPL_INVITING: return "341";
		case IrcResponseCode::RPL_SUMMONING: return "342";

		case IrcResponseCode::RPL_VERSION: return "351";
		case IrcResponseCode::RPL_WHOREPLY: return "352";
		case IrcResponseCode::RPL_NAMREPLY: return "353";
		case IrcResponseCode::RPL_LINKS: return "364";
		case IrcResponseCode::RPL_ENDOFLINKS: return "365";
		case IrcResponseCode::RPL_ENDOFNAMES: return "366";
		case IrcResponseCode::RPL_BANLIST: return "367";
		case IrcResponseCode::RPL_ENDOFBANLIST: return "368";
		case IrcResponseCode::RPL_INFO: return "371";
		case IrcResponseCode::RPL_MOTD: return "372";
		case IrcResponseCode::RPL_ENDOFINFO: return "374";
		case IrcResponseCode::RPL_MOTDSTART: return "375";
		case IrcResponseCode::RPL_ENDOFMOTD: return "376";
		case IrcResponseCode::RPL_YOUREOPER: return "381";
		case IrcResponseCode::RPL_REHASHING: return "382";
		case IrcResponseCode::RPL_TIME: return "391";
		case IrcResponseCode::RPL_USERSSTART: return "392";
		case IrcResponseCode::RPL_USERS: return "393";
		case IrcResponseCode::RPL_ENDOFUSERS: return "394";
		case IrcResponseCode::RPL_NOUSERS: return "395";
		default: return "999"; // Unknown code
	}
}
