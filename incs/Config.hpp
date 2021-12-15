#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <string>
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <iostream>

#include "Server.hpp"
#include "Location.hpp"
#include "Libft.hpp"


class Server;

class Config 
{
	private:
		// std::map<std::string, Server *> serverMap;

	public:
		void parsingConfig(std::string path);

};

#endif