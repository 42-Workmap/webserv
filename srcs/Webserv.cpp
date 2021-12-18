#include "../incs/Webserv.hpp"

Webserv::Webserv()
{
	this->m_fd_pool.resize(MAX_FD_SIZE, NULL);
	std::cout << "Webserv constructor called" << std::endl;
}

Webserv::Webserv(const Webserv &other)
{

}

Webserv::~Webserv()
{
	std::cout << "Webserv destructor called" << std::endl;
	// clean_everything();
}

Webserv &Webserv::operator=(const Webserv &other)
{
	this->m_kq = other.m_kq;
	this->m_fd_pool = other.m_fd_pool;
	for (int i = 0; i < MAX_FD_SIZE; i++)
	{
		this->m_return_events[i] = other.m_return_events[i];
	}
	return (*this);
}