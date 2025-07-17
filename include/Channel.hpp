#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ft_irc.hpp"

// maybe inheritance for safe channel and standard channel ?

class Channel
{
	private:
		std::string _name;
		// name
		// prefix (could also just be part of the name)
		// mode
			// current consideration is to a bool for every flag (Properties)
			// i(inv only),
			// t(topic settable only by operator),
			// k (password),
			// l (userlimit)
		bool	_inv;
		bool	_topic;
		bool	_pswrdTgle;
		bool	_usrLmtTgl;
		// password (if k is active)
		// userlimit
		// channel creator
		// channel operators
			// toggled with o
		// topic
		// number of users
		// list of users (map, *client)
		// list of operators ?
	public:
		Channel();
		~Channel();
};


#endif