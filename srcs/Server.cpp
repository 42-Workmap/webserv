#include "../incs/Server.hpp"

Server::Server()
{
	this->m_fd_type = FD_SERVER;
	this->m_port = -1;
}

Server::Server(const Server &other)
{
	this->m_fd = other.m_fd;
	this->m_fd_type = other.m_fd_type;
	this->m_ip = other.m_ip;
	this->m_port = other.m_port;
	this->m_server_name = other.m_server_name;
	this->m_locationMap.insert(other.m_locationMap.begin(), other.m_locationMap.end());

}
Server::~Server()
{

}

Server &Server::operator=(const Server &other)
{
	this->m_fd = other.m_fd;
	this->m_fd_type = other.m_fd_type;
	this->m_ip = other.m_ip;
	this->m_port = other.m_port;
	this->m_server_name = other.m_server_name;
	if (!this->m_locationMap.empty())
	{
		this->m_locationMap.clear();
	}
	this->m_locationMap.insert(other.m_locationMap.begin(), other.m_locationMap.end());
	return (*this);
}

void Server::setServerName(std::string server_name)
{
	this->m_server_name = server_name;
}

void Server::setIp(std::string ip)
{
	this->m_ip = ip;
}

void Server::setPort(std::string port)
{
	this->m_port = port;
}

const std::string &Server::getServerName(void) const
{
	return (this->m_server_name);
}

const std::string &Server::getIp(void) const
{
	return (this->m_ip);
}

const std::string &Server::getPort(void) const
{
	return (this->m_port);
}

std::map<std::string, Location> &Server::getLocations()
{
	return (this->m_locationMap);
}

Location& Server::getLastLocation(void)
{
	std::map<std::string, Location>::iterator it = m_locationMap.end();
	it--;
	return (it->second);
}

std::ostream &operator<<(std::ostream &o, Server &server)
{
	o << "==========Server==========" << std::endl;
	o << "IP: " << server.getIp() << std::endl;
	o << "Port: " << server.getPort() << std::endl;
	o << "Server_name: " << server.getServerName() << std::endl;

	o << "----------Location----------" << std::endl;
	for (std::map<std::string, Location>::iterator iter = server.getLocations().begin(); iter != server.getLocations().end(); iter++)
	{

		o << "maxbody: " << iter->second.getMaxBodySize() << std::endl;
		o << "returnNum: " << iter->second.getReturnNum() << std::endl;
		o << "autoindex: " << iter->second.getAutoIndex() << std::endl;
		o << "root: " << iter->second.getRoot() << std::endl;
		o << "Uri: " << iter->second.getUri() << std::endl;
		o << "authkey: " << iter->second.getAuthKey() << std::endl;
		o << "returnUrl: " << iter->second.getReturnUrl() << std::endl;


		o << "AllowMethods: ";
		for (std::vector<std::string>::iterator i = iter->second.getAllowMethods().begin(); i != iter->second.getAllowMethods().end(); i++)
		{
			o << *i << " ";
		}
		o << std::endl << "Indexs: ";
		for (std::vector<std::string>::iterator i = iter->second.getIndexs().begin(); i != iter->second.getIndexs().end(); i++)
		{
			o << *i << " ";
		}
		o << std::endl << "ErrorPages : ";
		for (std::map<int, std::string>::iterator i = iter->second.getErrorPages().begin(); i != iter->second.getErrorPages().end(); i++)
		{
			o << (i->first) << " "<< (i->second) << " ";
		}

		o << std::endl << "Cgis : ";
		for (std::map<std::string, std::string>::iterator i = iter->second.getCgi().begin(); i != iter->second.getCgi().end(); i++)
		{
			o << (i->first) << " "<< (i->second) << " ";
		}
		o << std::endl;
	}
	o << std::endl;

	return o;
}

Location	&Server::getPerfectLocation(std::string &uri)
{
	Location *ret = &(this->m_locationMap["/"]);
	std::string key = "";
	for (std::string::const_iterator iter = uri.begin(); iter != uri.end(); iter++)
	{
		key += *iter;
		if (*iter == '/')
		{
			if (this->m_locationMap.find(key) == this->m_locationMap.end())
				return (*ret);
			else
				ret = &(this->m_locationMap[key]);
		}
	}
	if ( *(--key.end()) != '/') // '/'로 끝나지 않았고
	{
		key += '/';
		if (this->m_locationMap.find(key) != this->m_locationMap.end())
		{
			uri = key;
			return (this->m_locationMap[key]);
		}
	}
	return (*ret);
}