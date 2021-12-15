#include "../incs/FdBase.hpp"

FdBase::FdBase(void) : m_fd(-1)
{
}

FdBase::FdBase(const FdBase &other)
{
	*this = other;
}

FdBase::~FdBase(void)
{

}

FdBase &FdBase::operator=(const FdBase &other)
{
	this->m_fd = other.getFd();
	this->m_fd_type = other.getFdType();
	return (*this);
}

int FdBase::getFd(void) const
{
	return (this->m_fd);
}

e_fd_type FdBase::getFdType(void) const
{
	return (this->m_fd_type);
}

void FdBase::setFd(int fd)
{
	this->m_fd = fd;
}

void FdBase::setFdType(e_fd_type fd_type)
{
	this->m_fd_type = fd_type;
}