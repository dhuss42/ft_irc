/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 14:12:23 by dhuss             #+#    #+#             */
/*   Updated: 2025/09/08 17:35:45 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// check for two args
// handle args
// 		1. port number
//			check if it's only digits
//			convert to int
//			valid range from 1 to 65535
// 			disallow Well known ports 0-1023
// 		2. connection password
// 			allow only printable ASCII
// parse arguments
// start server loop


// handle signals here or inside the Server class
// 	SIGINT
//

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
		Errors::handleErrors(e);
	}
	return (0);
}
