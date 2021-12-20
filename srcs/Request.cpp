#include "../incs/Request.hpp"

Request::Request()
{

}

Request::~Request()
{

}

Request::Request(const Request& other)
{
    *this = other;
}

Request& Request::operator=(const Request& other)
{
    m_origin = other.m_origin;
    m_body = other.m_body;
    m_method = other.m_method;
    m_reqlocation = other.m_reqlocation;
    m_httpver = other.m_httpver;
    for (std::map<std::string, std::string>::const_iterator it = other.m_headersMap.begin(); it != other.m_headersMap.end(); it++)
    {
        m_headersMap[it->first] = it->second;
    }
    return *this;
}

std::string& Request::getOrigin()
{
    return m_origin;
}

std::string& Request::getBody()
{
    return m_body;
}

std::string& Request::getMethod()
{
    return m_method;
}

std::string& Request::getReqLocation()
{
    return m_reqlocation;
}

std::string& Request::getHttpVer()
{
    return m_httpver;
}

std::map<std::string, std::string> & Request::getHeadersMap()
{
    return m_headersMap;
}

std::string& Request::getRequestStatus()
{
    return m_request_status;
}

Client* Request::getClient()
{
    return m_client;
}

size_t Request::getRemainBodyValue()
{
    return m_remain_body_value;
}


void Request::setOrigin(std::string origin)
{
    m_origin = origin;
}

void Request::setBody(std::string body)
{
    m_body = body;
}

void Request::setMethod(std::string method)
{
    m_method = method;
}

void Request::setReqLocation(std::string location)
{
    m_reqlocation = location;
}

void Request::setHttpver(std::string httpver)
{
    m_httpver = httpver;
}

void Request::setRequestStatus(std::string status)
{
    m_request_status = status;
}

void Request::setClient(Client* client)
{
    m_client = client;
}

void Request::setRemainBodyValue(size_t len)
{
    m_remain_body_value = len;
}
