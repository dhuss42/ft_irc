
#include "ft_irc.hpp"

void toLower(std::string& str)
{
	for (unsigned long i = 0; i < str.length(); i++)
		str[i] = std::tolower(str[i]);
}

// Method for veryifing correct nick adhering to the following rules
// - not empty
// - no " ", ",", "!", "*", "?", "@"
// - cannot start with $ or :
// - cannot start with char #&!+
// - channel Member Prefixes: "~", "+q", "&", "+a", "@", "+o", "%", "+h", "+", "+v"
// - no '.'

//==================NICK==================//

bool	verifyNickName(const std::string& name)
{
	if (name.empty() || name.size() > 15)
		return (false);
	char first = name[0];
	if (first == '$' || first == '|' || first == '#' || first == '&' || first == '+' || first == '%' || first == '~' || std::isdigit(first))
		return (false);
	// check if all chars are alnum
	if (name.find(' ') != std::string::npos ||
		name.find(',') != std::string::npos ||
		name.find('!') != std::string::npos ||
		name.find('*') != std::string::npos ||
		name.find('?') != std::string::npos ||
		name.find('@') != std::string::npos ||
		name.find('.') != std::string::npos)
		// use switch case and iterationr ather than find for all of them
		return (false);
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
