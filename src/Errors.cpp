/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 14:12:41 by dhuss             #+#    #+#             */
/*   Updated: 2025/07/07 16:02:55 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// should slipt into errors critical for server infrastructure
// which should result in closing the program 
// and errors that should simple be handled gracefully without a shutdown

#include "Errors.hpp"

Errors::Errors(ErrorCode code) : std::runtime_error(make_message(code)), _code(code)
{
}

Errors::~Errors()
{
}

std::string Errors::make_message(ErrorCode code)
{
	switch (code)
	{
		case ErrorCode::E_ARGNBR:
			return ("invalid number of arguments");
		case ErrorCode::E_PRT:
			return ("invalid port");
		case ErrorCode::E_PRTNBR:
			return ("invalid port number");
		case ErrorCode::E_PSSWRD:
			return ("invalid password format");
		case ErrorCode::E_SCKFD:
			return ("socket error");
		case ErrorCode::E_FCNTL:
			return ("fcntl error");
		case ErrorCode::E_BND:
			return ("bind error");
		case ErrorCode::E_LSTN:
			return ("listen error");
		case ErrorCode::E_PLL:
			return ("pull error");
		case ErrorCode::E_ACCPT:
			return ("accept error");
		case ErrorCode::E_SCKEMPTY:
			return ("socket vector empty");
		case ErrorCode::E_RCV:
			return ("recv error");
		case ErrorCode::E_SND:
			return ("send error");
		default:
			return ("Unknown error");
	}
}

/*-----------------------------------------------------------------------*/
/* Errors are caught as exception as Errors inherits from std::exception */
/* dynamic_cast downcasts												 */
/* if e is an Errors object then it returns a pointer to it				 */
/* -> custom error														 */
/* else it returns a nullptr											 */
/* -> Unexpected error caught											 */
/*-----------------------------------------------------------------------*/
void	Errors::handleErrors(const std::exception& e)
{
	const Errors* customError = dynamic_cast<const Errors*>(&e);

	if (customError)
	{
		std::cerr << RED "Error: " << customError->what() << WHITE << std::endl;
		switch (customError->_code)
		{
			case ErrorCode::E_ARGNBR:
				std::cerr << BOLDCYAN "\tType ./ircserv \"portnumber\" \"password\"" RESET << std::endl;
				break ;
			case ErrorCode::E_PRT:
				std::cerr << BOLDCYAN "\tPort number may only consist of digits" RESET << std::endl;
				break ;
			case ErrorCode::E_PRTNBR:
				std::cerr << BOLDCYAN "\tUsable ports range from 1024 - 65535" RESET << std::endl;
				break ;
			case ErrorCode::E_PSSWRD:
				std::cerr << BOLDCYAN "\tPassword may only be printable chars" RESET << std::endl;
				break ;
			default:
				std::cerr << BOLDCYAN "\tDefault error" RESET << std::endl;
				break ;
		}
	}
	else
		std::cerr << RED "Error: " << e.what() << WHITE << std::endl;
	std::exit(EXIT_FAILURE);
}
