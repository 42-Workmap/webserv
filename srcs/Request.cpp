#include "../incs/Request.hpp"

Request::Request()
{
    this->m_request_status = HEADER_PARSING;
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

e_req_status& Request::getRequestStatus()
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

void Request::setRequestStatus(e_req_status status)
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

std::string trim(std::string& str)
{
    std::size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    std::size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool Request::makeHeader(void)
{
    std::cout << "MakeHEADER()" << std::endl;
    std::size_t found = this->m_origin.find("\r\n");
    std::vector<std::string> vFirsts;
    std::string firstline = this->m_origin.substr(0, found);
    ft_split(firstline, " ", vFirsts);
    this->setMethod(vFirsts[0]);
    this->setReqLocation(vFirsts[1]);
    this->setHttpver(vFirsts[2]);
    if (this->m_origin.length() >= found+2)
        this->setOrigin(this->m_origin.substr(found+2));
    else
        this->setOrigin("");

    found = m_origin.find("\r\n\r\n");
    std::string tempheader = m_origin.substr(0, found);
    std::vector<std::string> tempVHeaders;
    ft_split(tempheader, "\r\n", tempVHeaders);
    for (std::vector<std::string>::iterator it = tempVHeaders.begin(); it != tempVHeaders.end(); it++)
    {
        // Content-Length  :   120\r\n
        std::string line = *it;
        std::size_t idx = line.find(':');
        std::string key = line.substr(0, idx);
        std::string value = "";
        if (line.length() != (found+1))
            value = line.substr(idx+1);
        m_headersMap[trim(key)] = trim(value);
    }
    if (this->m_origin.length() >= found+4)
        this->setOrigin(this->m_origin.substr(found+4));
    else
        this->setOrigin("");

    //test
    for (std::map<std::string, std::string>::iterator it = m_headersMap.begin(); it != m_headersMap.end(); it++)
    {
        std::cout << it->first <<" "<< it->second << std::endl;
    }
    return true;
}
bool Request::makeBody(void)
{
    if (m_request_status == CONTENT_BODY)
    {
        std::cout << "ContentBody()" << std::endl;
        if (m_remain_body_value <= m_origin.length())
        {
            m_body.append(m_origin.substr(0, m_remain_body_value));
            m_origin.erase(0, m_remain_body_value);
            m_remain_body_value = 0;
            return (checkValidRequest("FINISHED"));
        }
        return (false);
    }
    else if (m_request_status == CHUCKED)
    {
        std::cout << "Chucked()" << std::endl;
        size_t tmp;
        std::stringstream ss;
        std::size_t found = m_origin.find("\r\n");
        if (found == std::string::npos)
            return false;
        else
        {
            ss << std::hex << m_origin.substr(0, found);
            ss >> tmp;
            m_remain_body_value = tmp;
            m_origin = m_origin.substr(found+2);
            m_request_status = CHUCKED_BODY;
            return (makeBody());
        }
    }
    else if (CHUCKED_BODY)
    {
        if (m_remain_body_value <= m_origin.size())
        {
            size_t tmp = m_origin.size();
            m_body.append(m_origin.substr(0, m_remain_body_value -2));
            m_origin.erase(0, m_remain_body_value);

            m_remain_body_value = 0;
            if (m_body.size() == tmp)
            {
                return (checkValidRequest("FINISHED"));
            }
            m_request_status = CHUCKED;
            return (makeBody());
        }
    }
    return (checkValidRequest("FINISHED"));
}

bool Request::checkValidRequest(std::string fin)
{
    if (fin != "FINISHED")
        return false;

    // 파싱이 끝났으면 올바른지 확인하는 코드가 밑에 있다 


    setRequestStatus(HEADER_PARSING);
    return true;
}