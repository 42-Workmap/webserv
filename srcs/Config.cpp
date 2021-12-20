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

Server* Config::getLastServer(void)
{
	std::map<std::string, Server>::iterator it = m_server_map.end();
	it--;
	return &(it->second);
}


void Config::setWebserv(Webserv* webserv)
{
	this->m_webserv = webserv;
}

bool	Config::isKeyword(std::string keyword)
{
	if (keyword == "server" ||
		keyword == "listen" ||
		keyword == "server_name" ||
		keyword == "location" ||
		keyword == "error_page" ||
		keyword == "allow_methods" ||
		keyword == "root" ||
		keyword == "index" ||
		keyword == "upload_path" ||
		keyword == "auto_index" ||
		keyword == "request_max_body_size" ||
		keyword == "auth_key" ||
		keyword == "cgi_info" ||
		keyword == "return" ||
		keyword == "}" ||
		keyword == "{" )
		return (true);
	return (false);
}

void 	Config::parsingConfig(std::string path)
{
	std::ifstream output;
	std::string lines;
	std::string temp;
	std::vector<std::string> vinfos;
	Server *server;
	Location *location;

	output.open(path, std::ofstream::in);
	if (output.fail())
	{
		std::cerr << "ERROR" << std::endl;
		exit(1);
	}
	while (getline(output, temp))
	{
		lines += temp;
	}
	ft_split(lines, " \t}{", vinfos);
	for (std::vector<std::string>::const_iterator it = vinfos.begin(); it != vinfos.end(); it++)
	{
		if (*it == "server_name")
		{
			it++;
			std::string server_name = *it;
			it++;
			it++;
			std::string port = *it;
			it++;
			std::string ip = *it;
			std::string key = ip +":"+ port;
			if (getServerMap().find(key) != getServerMap().end())
			{
				throw "Duplicated Server Ip:Port";
			}
			getServerMap()[key].setServerName(server_name);
			getServerMap()[key].setIp(ip);
			getServerMap()[key].setPort(port);
		}
		if (*it == "location")
		{
			server = getLastServer();
			it++;
			std::string uri = *it;
			if (server->getLocations().find(uri) != server->getLocations().end())
			{
				throw "Duplicated Location Uri";
			}
			server->getLocations()[uri].setUri(uri);
			location = &(server->getLocations()[uri]);
		}
		if (*it == "error_page")
		{
			it++;
			int ernum = std::atoi((*it).c_str());
			it++;
			location->getErrorPages()[ernum] = *it;
		}
		if (*it == "allow_methods")
		{
			it++;
			std::vector<std::string> gpd;
			gpd.push_back("GET");
			gpd.push_back("POST");
			gpd.push_back("DELETE");
			while (std::find(gpd.begin(), gpd.end(), *it) != gpd.end())
			{
				location->getAllowMethods().push_back(*it);
				it++;
			}
			it--;
		}
		if (*it == "root")
		{
			it++;
			location->setRoot(*it);
		}
		if (*it == "index")
		{
			it++;
			while(!isKeyword(*it))
			{
				location->getIndexs().push_back(*it);
				it++;
			}
			it--;
		}
		if (*it == "auto_index")
		{
			it++;
			if (*it == "on")
				location->setAutoIndex(true);
			else
				location->setAutoIndex(false);
		}
		if (*it == "cgi_info")
		{
			it++;
			std::string cgikey = *it;
			it++;
			location->getCgi()[cgikey] = *it;

		}
		if (*it == "auth_key")
		{
			it++;
			location->setAuthKey(*it);
		}
		if (*it == "return")
		{
			it++;
			location->setReturnNum(std::atoi((*it).c_str()));
			it++;
			location->setReturnUrl(*it);
		}
		if (*it == "request_max_body_size")
		{
			it++;
			location->setMaxBodySize(std::atoi((*it).c_str()));
		}


	}
	for (std::map<std::string, Server>::iterator k = getServerMap().begin(); k != getServerMap().end(); k++)
	{
		std::cout << k->second;
	}

	output.close();
}