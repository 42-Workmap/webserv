#include "../incs/Config.hpp"

Config* Config::m_config = 0; // static 멤버 변수 초기화 

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

Config* Config::getConfig(void)
{
	if (m_config == NULL)
	{
		m_config = new Config();
	}
	return m_config;
}

Webserv* Config::getWebserv()
{
	return this->m_webserv;
}

std::map<std::string, Server> &Config::getServerMap()
{
	return this->m_server_map;
}

std::map<std::string, std::string> &Config::getMimeType()
{
	return this->m_mime_type;
}

std::map<std::string, std::string> &Config::getStatusCode()
{
	return this->m_status_code;
}

Server& Config::getLastServer(void)
{
	std::map<std::string, Server>::iterator it = m_server_map.end();
	it--;
	return (it->second);
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
	std::vector<std::string> vinfos;

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
	ft_split(lines, " \t", vinfos);
	for (std::vector<std::string>::const_iterator it = vinfos.begin(); it != vinfos.end(); it++)
	{
		if (*it == "server_name")
		{
			*it++;
			std::string server_name = *it;
			*it++;
			*it++;
			std::string port = *it;
			*it++;
			std::string ip = *it;
			std::string key = ip +":"+ port;
			if (getServerMap().find(key) != getServerMap().end())
			{
				throw "Duplicated Server Ip:Port";
			}
			getServerMap()[key].setServerName(server_name);
			getServerMap()[key].setIp(ip);
			getServerMap()[key].setPort(port);
			std::cout << getLastServer();
		}
		if (*it == "location")
		{

		}
	}
	


	output.close();
}