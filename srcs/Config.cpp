#include "../incs/Config.hpp"

void 	Config::parsingConfig(std::string path)
{
	std::ifstream output;
	std::string lines;
	std::string temp;
	std::vector<std::string> infos;

	output.open(path, std::ofstream::in);
	if (output.fail())
	{
		std::cerr << "ERROR" << std::endl;
		exit(1);
	}
	while (getline(output, temp))
	{
		std::cout << temp << std::endl;
		lines += temp;
	}
	ft_split(lines, " \t", infos);
	for (std::vector<std::string>::const_iterator it = infos.begin(); it != infos.end(); it++)
	{
		std::cout << *it << " ";
	}
	output.close();
}