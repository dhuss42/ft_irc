
#include "ft_irc.hpp"

std::string toLower(const std::string& str)
{
	std::string lowerCase = str;
	for (unsigned long i = 0; i < str.length(); i++)
		lowerCase[i] = std::tolower(str[i]);
	return (lowerCase);
}

//==================NICK==================//

/*------------------------------------------------------------------*/
/*	used for NICK command to verify name							*/
/*		- checks if empty or too long								*/
/*		- checks for invalid first chars like channel prefix		*/
/*		- check's if all chars in name are valid					*/
/*------------------------------------------------------------------*/
bool	verifyNickName(const std::string& name)
{
	if (name.empty() || name.size() > 15)
		return (false);
	char first = name[0];
	if (first == '$' || first == '|' || first == '#' || first == '&' || first == '+' || first == '%' || first == '~' || std::isdigit(first))
		return (false);
	for (const char &c : name)
	{
		switch (c)
		{
			if (std::isalnum(static_cast<unsigned char> (c)))
				continue;
			case ('['):
			case (']'):
			case ('{'):
			case ('}'):
			case ('\\'):
			case ('|'):
				continue;

		default:
			return (false);
		}
	}
	return (true);
}

//==================JOIN==================//

bool	verifyChannelName(const std::string& name)
{
	if (name.empty() || name.size() >= 50)
		return (false);
	if (name[0] == '#' || name[0] == '&' || name[0] == '+' || name[0] == '!')
	{
		if (name.find(' ') != std::string::npos ||
			name.find(',') != std::string::npos ||
			name.find(7) != std::string::npos)
			return (false);
	}
	return (true);
}
