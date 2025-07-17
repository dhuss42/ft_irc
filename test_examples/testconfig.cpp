#include "../include/ft_irc.hpp"
#include <fstream>

int	main()
{
	std::ifstream file("ircserv.conf");
	std::string line;
	std::regex pattern("(\\w+)\\s*=\\s*(.+)$");
	std::smatch matches;

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			if (std::regex_match(line, matches, pattern))
			{
				std::cout << "Key: " << matches[1].str() << std::endl;
				std::cout << "Value: " << matches[2].str() << std::endl;
			}
		}
		file.close();
	}
	else
		std::cerr << "Error opening file" << std::endl;
	
	
}

// skipp comments
// trim whitespace around key and value especially trailing ones
// Handle case sensitivity
// handle duplicate keys
// Handle missing essential keys -> maybe simply set to default value
// Invalid Formats
// Type valdiation (boolian, int)
// Paths relative and absolute
// Defaults for optional configs

// could implement a config parser class that returns a map at the end and the server object could take this optionally
