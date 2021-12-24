#include "../incs/Client.hpp"

Client::Client()
{
	this->m_server = 0;
	this->m_c_status = REQUEST_RECEIVING;
	this->m_fd_type = FD_CLIENT;
	this->m_fd = -1;
}

Client::Client(Server *server, int c_fd)
{
	this->m_server = server;
	this->m_c_status = REQUEST_RECEIVING;
	this->m_fd_type = FD_CLIENT;
	this->m_fd = c_fd;
}
Client::~Client()
{

}

Client::Client(const Client &other)
{
	*this = other;
}

Client &Client::operator=(const Client &other)
{
	this->m_server = other.m_server;
	this->m_c_status = other.m_c_status;
	this->m_fd_type = other.m_fd_type;
	this->m_fd = other.m_fd;
	return (*this);
}

Request &Client::getRequest(void)
{
	return (this->m_request);
}

Response &Client::getResponse(void)
{
	return (this->m_response);
}

const e_c_status Client::getCStatus(void) const
{
	return (this->m_c_status);
}

const struct timeval Client::getLastTime(void) const
{
	return (this->m_last_time);
}

Server* Client::getServer(void)
{
	return (this->m_server);
}

void Client::setLastTime(struct timeval last_time)
{
	this->m_last_time = last_time;
}

void Client::setCStatus(e_c_status c_status)
{
	this->m_c_status = c_status;
}

void Client::appendOrigin(std::string newstr)
{
	this->getRequest().getOrigin() += newstr;
}

bool Client::parseRequest()
{
	int idx = m_request.getOrigin().find("\r\n\r\n");
	if (idx == std::string::npos)
		return false;
	m_request.makeHeader(); // startline, header
		


	
	
}