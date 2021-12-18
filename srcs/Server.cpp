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