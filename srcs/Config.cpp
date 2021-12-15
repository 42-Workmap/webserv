#include "../incs/Config.hpp"

Config::Config()
{
}

Config::~Config()
{
	delete this->m_config;
}

// Config::Config(const Config &other)
// {
// 	*this = other;
// }

// Config& Config::operator=(const Config & other)
// {
// 	this->m_webserv = other.m_webserv;
// 	this->m_server_map = other.m_server_map;
// 	this->m_mime_type = other.m_mime_type;
// 	this->m_status_code = other.m_status_code;
// 	return *this;
// }

Webserv* Config::getWebserv()
{
	return this->m_webserv;
}

std::map<std::string, Server> Config::getServerMap()
{
	return this->m_server_map;
}

std::map<std::string, std::string> Config::getMimeType()
{
	return this->m_mime_type;
}

std::map<std::string, std::string> Config::getStatusCode()
{
	return this->m_status_code;
}


void Config::setWebserv(Webserv* webserv)
{
	this->m_webserv = webserv;
}


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