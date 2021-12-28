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

e_c_status &Client::getCStatus(void)
{
	return (this->m_c_status);
}

struct timeval &Client::getLastTime(void)
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
	m_request.setClient(this);  //
	if (m_request.getRequestStatus() == HEADER_PARSING)
	{
		std::size_t idx = m_request.getOrigin().find("\r\n\r\n");
		if (idx == std::string::npos)
			return false;
		// m_request.makeStartLine();
		m_request.makeHeader(); // startline, header
		m_request.setRequestStatus(BODY_PARSING);
	}
	if (m_request.getRequestStatus() == BODY_PARSING)
	{
		if ((m_request.getHeadersMap().count("Transfer-Encoding") == 1) && \
		(m_request.getHeadersMap()["Transfer-Encoding"] == "chunked"))
		{
			m_request.setRequestStatus(CHUNKED);
			return (m_request.makeBody());
		}
		else if (m_request.getHeadersMap().count("Content-Length"))
		{
			m_request.setRemainBodyValue(atoi(m_request.getHeadersMap()["Content-Length"].c_str()));
			if (m_request.getRemainBodyValue() == 0)
			{
				return (m_request.checkValidRequest("FINISHED"));
			}
			m_request.setRequestStatus(CONTENT_BODY);
			return (m_request.makeBody());
		}
		else
			return (m_request.checkValidRequest("FINISHED"));
	}
	
	return (m_request.makeBody());
}