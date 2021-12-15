#include "../incs/Libft.hpp"

void	ft_split(const std::string &target, const std::string &sep, std::vector<std::string> &infos)
{
	std::string word;

	for (std::string::const_iterator it = target.begin(); it != target.end(); it++)
	{
		if (sep.find(*it) == std::string::npos)
		{
			word += *it;
		}
		else 
		{
			if (word != "")
			{
				infos.push_back(word);
				word.clear();
			}
		}
	}
	if (word != "")
	{
		infos.push_back(word);
		word.clear();
	}
}