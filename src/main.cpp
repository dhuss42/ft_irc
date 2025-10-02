/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 14:12:23 by dhuss             #+#    #+#             */
/*   Updated: 2025/10/02 14:22:56 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	main(int argc, char*argv[])
{
	try {
		if (argc == 3)
		{
			Server server(argv[1], argv[2]);
			Server::setupSignalHandler();
			server.serverLoop();
		}
		else
			throw (Errors(ErrorCode::E_ARGNBR));
	}
	catch (const std::exception& e) {
		Errors::handleErrors(e, nullptr);
	}
	return (0);
}
