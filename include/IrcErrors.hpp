/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcErrors.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 15:51:31 by maustel           #+#    #+#             */
/*   Updated: 2025/09/17 15:51:31 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCERRORS_HPP
# define IRCERRORS_HPP

# include "ft_irc.hpp"

enum class IrcErrorCode
{
	ERR_NOSUCHNICK,		// No such nick/channel
	ERR_NOSUCHSERVER,	// No such server
	ERR_NOSUCHCHANNEL,	// No such channel
	ERR_CANNOTSENDTOCHAN,// Cannot send to channel
	ERR_TOOMANYCHANNELS,// Too many channels
	ERR_WASNOSUCHNICK,	// There was no such nick
	ERR_TOOMANYTARGETS,	// Too many targets
	ERR_NOORIGIN,		// No origin specified
	ERR_INVALIDCAPCMD,	// Invalid or missing CAP subcommand
	ERR_NORECIPIENT,	// No recipient given
	ERR_NOTEXTTOSEND,	// No text to send
	ERR_NOTOPLEVEL,		// No top level domain
	ERR_WILDTOPLEVEL,	// Wild card top level domain
	ERR_BADMASK,		// Invalid mask
	ERR_UNKNOWNCOMMAND,	// Unknown command
	ERR_NOMOTD,			// MOTD file missing
	ERR_NOADMININFO,	// No admin information available
	ERR_FILEERROR,		// File error
	ERR_NONICKNAMEGIVEN,// No nickname given
	ERR_ERRONEUSNICKNAME,// Erroneous nickname
	ERR_NICKNAMEINUSE,	// Nickname is already in use
	ERR_NICKCOLLISION,	// Nickname collision
	ERR_UNAVAILRESOURCE,// Nick/channel is temporarily unavailable
	ERR_USERNOTINCHANNEL,// They aren't on that channel
	ERR_NOTONCHANNEL,	// You're not on that channel
	ERR_USERONCHANNEL,	// User is already on channel
	ERR_NOLOGIN,		// User not logged in
	ERR_SUMMONDISABLED,	// SUMMON command disabled
	ERR_USERSDISABLED,	// USERS command disabled
	ERR_NOTREGISTERED,	// You have not registered
	ERR_NEEDMOREPARAMS,	// Not enough parameters --
	ERR_ALREADYREGISTRED,// Already registered
	ERR_NOPERMFORHOST,	// No permission for operation
	ERR_PASSWDMISMATCH,	// Password mismatch
	ERR_YOUREBANNEDCREEP,// You're banned
	ERR_KEYSET,			// Channel key already set
	ERR_CHANNELISFULL,	// Channel is full
	ERR_UNKNOWNMODE,	// Unknown mode
	ERR_INVITEONLYCHAN,	// Invite-only channel
	ERR_BANNEDFROMCHAN,	// Banned from channel
	ERR_BADCHANNELKEY,	// Bad channel key
	ERR_BADCHANMASK,	// Bad channel mask
	ERR_NOCHANMODES,	// Channel modes can't be changed
	ERR_BANLISTFULL,	// Ban list is full
	ERR_NOPRIVILEGES,	// No privileges
	ERR_CHANOPRIVSNEEDED,// You're not channel operator
	ERR_CANTKILLSERVER,	// Can't kill server
	ERR_RESTRICTED,		// Restricted command
	ERR_UNIQOPPRIVSNEEDED,// You need uniqop privileges
	ERR_NOOPERHOST,		// No O-line for your host
	ERR_UMODEUNKNOWNFLAG,// Unknown MODE flag
	ERR_USERSDONTMATCH,	// Cannot change or set mode for other
	DEFAULT
};

const char* getIrcErrorCodeString(IrcErrorCode code);
const std::string getIrcErrorReply(IrcErrorCode code);

#endif


