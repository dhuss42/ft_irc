/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcResponses.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 16:06:21 by maustel           #+#    #+#             */
/*   Updated: 2025/09/17 16:06:21 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCRESPONSES_HPP
# define IRCRESPONSES_HPP

#include "ft_irc.hpp"

enum class IrcResponseCode
{
	RPL_WELCOME,
	RPL_YOURHOST,
	RPL_CREATED,
	RPL_MYINFO,
	RPL_ISUPPORT,

	RPL_BOUNCE,
	RPL_TRACELINK,
	RPL_TRACECONNECTING,
	RPL_TRACEHANDSHAKE,
	RPL_TRACEUNKNOWN,
	RPL_TRACEOPERATOR,
	RPL_TRACEUSER,
	RPL_TRACESERVER,
	RPL_TRACESERVICE,
	RPL_TRACENEWTYPE,
	RPL_TRACECLASS,

	RPL_STATSLINKINFO,
	RPL_STATSCOMMANDS,
	RPL_STATSCLINE,
	RPL_STATSNLINE,
	RPL_STATSILINE,
	RPL_STATSKLINE,
	RPL_STATSYLINE,
	RPL_ENDOFSTATS,

	RPL_UMODEIS,
	RPL_SERVLIST,
	RPL_SERVLISTEND,

	RPL_STATSLLINE,
	RPL_STATSUPTIME,
	RPL_STATSOLINE,
	RPL_STATSHLINE,
	RPL_STATSSLINE,
	RPL_STATSPING,
	RPL_STATSBLINE,

	RPL_LUSERCLIENT,
	RPL_LUSEROP,
	RPL_LUSERUNKNOWN,
	RPL_LUSERCHANNELS,
	RPL_LUSERME,

	RPL_ADMINME,
	RPL_ADMINLOC1,
	RPL_ADMINLOC2,
	RPL_ADMINEMAIL,

	RPL_TRACELOG,
	RPL_TRACEEND,
	RPL_TRYAGAIN,

	RPL_NONE,
	RPL_AWAY,
	RPL_USERHOST,
	RPL_ISON,
	RPL_UNAWAY,
	RPL_NOWAWAY,
	RPL_WHOISUSER,
	RPL_WHOISSERVER,
	RPL_WHOISOPERATOR,
	RPL_WHOWASUSER,
	RPL_ENDOFWHO,
	RPL_WHOISIDLE,
	RPL_ENDOFWHOIS,
	RPL_WHOISCHANNELS,

	RPL_LISTSTART,
	RPL_LIST,
	RPL_LISTEND,
	RPL_CHANNELMODEIS,
	RPL_UNIQOPIS,
	RPL_NOTOPIC,
	RPL_TOPIC,
	RPL_INVITING,
	RPL_SUMMONING,

	RPL_VERSION,
	RPL_WHOREPLY,
	RPL_NAMREPLY,
	RPL_LINKS,
	RPL_ENDOFLINKS,
	RPL_ENDOFNAMES,
	RPL_BANLIST,
	RPL_ENDOFBANLIST,
	RPL_INFO,
	RPL_MOTD,
	RPL_ENDOFINFO,
	RPL_MOTDSTART,
	RPL_ENDOFMOTD,
	RPL_YOUREOPER,
	RPL_REHASHING,
	RPL_TIME,
	RPL_USERSSTART,
	RPL_USERS,
	RPL_ENDOFUSERS,
	RPL_NOUSERS,
	DEFAULT
};

const char* getIrcResponseCodeString(IrcResponseCode code);

#endif
