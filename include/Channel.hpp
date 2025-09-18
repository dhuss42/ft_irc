#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ft_irc.hpp"
#include "Client.hpp"

// maybe inheritance for safe channel and standard channel ?

class Client;

class Channel
{
	private:
		std::string _name;
		std::string _topic;

		// modes
		bool	_invOnly; // i(inv only),
		bool	_topicOp; // t(topic settable only by operator),
		bool	_pswrdTgle; // k (password),
		bool	_usrLmtTgl; // l (userlimit)

		std::string _password; // if k is active
		std::size_t _userLimit;

		std::string _creator; // uncertain if needed
		std::map <std::string, Client*> _users;
		std::unordered_map <std::string, Client*> _invitedUsers;
		std::unordered_map <std::string, Client*> _operators;

	public:
		Channel(std::string name);
		~Channel();

		// ======= Adding and Removing Clients =======//
		void	addUser(Client* client, const std::string& password);
		void	addOperator(Client* client);
		void	addInvUsers(Client* client);

		void	inviteUser(Client* inviter, Client* invited);
		void	kickUser(Client* kicker, const std::string& kicked);

		void	removeUser(Client* client);
		void	removeOperator(Client* client);
		void	removeInvUsers(Client* client);

		// ======= send Messages =======//
		void	broadcast(const std::string& msg, Client* sender);

		void	changeTopic(const std::string& topic, const Client* client);

		// ======= verify Clients =======//
		bool	isOperator(const Client* client) const;
		bool	isUser(const Client* client) const;
		bool	isInvited(const Client* client) const;

		// ======= setters & getters =======//
		std::string	getName(void);
		std::string	getTopic(void);
		std::string	getPassword(void);
		std::size_t	getUserLimit(void);

		const std::string getJoinedUsers(void) const;

		void		setName(std::string name); // might not need this as channel names are fixed
		void		setTopic(std::string topic);
		void		setPassword(std::string password);
		void		setUserLimit(std::size_t userLimit);

		// ======= set and get modes =======//
		bool	getInvOnly(void);
		bool	getTopicOp(void);
		bool	getPasswordToggle(void);
		bool	getUserLimitToggle(void);

		void	setInvOnly(bool value);
		void	setTopicOp(bool value);
		void	setPasswordToggle(bool value);
		void	setUserLimitToggle(bool value);


		void	printUsers(void);

	};


#endif