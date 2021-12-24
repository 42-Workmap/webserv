#include "../incs/Response.hpp"

Response::Response()
{

}

Response::~Response()
{

}

Response::Response(const Response &other)
{
    *this = other;
}

Response& Response::operator=(const Response &other)
{
    m_return = other.m_return;
    m_disconnect = other.m_disconnect;
    // m_client = m_client;
    m_origin = other.m_origin;
    m_resource_path = other.m_resource_path;
    // m_location = other.m_location;

    m_cgi_extention = other.m_cgi_extention;
    m_write_idx = other.m_write_idx;

    m_fd_read = other.m_fd_read;
    m_fd_write = other.m_fd_write;
    return (*this);
}

bool Response::getReturn()
{
    return m_return;
}

bool Response::getDisconnect()
{
    return m_disconnect;
}

Client* Response::getClient()
{
    return m_client;
}

std::string& Response::getOrigin()
{
    return m_origin;
}

std::string Response::getResourcePath()
{
    return m_resource_path;
}

Location* Response::getLocation()
{
    return m_location;
}

std::string Response::getCgiExtention()
{
    return m_cgi_extention;
}

size_t Response::getWriteIdx()
{
    return m_write_idx;
}

int Response::getFdRead()
{
    return m_fd_read;
}

int Response::getFdWrite()
{
    return m_fd_write;
}

void Response::setReturn(bool ret)
{
    m_return = ret;
}

void Response::setDisconnect(bool disconnect)
{
    m_disconnect = disconnect;
}

void Response::setClient(Client* client)
{
    m_client = client;
}

void Response::setOrigin(std::string origin)
{
    m_origin = origin;
}

void Response::setResourcePath(std::string path)
{
    m_resource_path = path;
}

void Response::setLocation(Location *location)
{
    m_location = location;
}

void Response::setCgi(std::string cgi)
{
    m_cgi_extention = cgi;
}

void Response::setWriteIdx(size_t idx)
{
    m_write_idx = idx;
}

void Response::setFdRead(int fd)
{
    m_fd_read = fd;
}

void Response::setFdWrite(int fd)
{
    m_fd_write = fd;
}
