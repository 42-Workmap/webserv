#include "../incs/Request.hpp"
#include "../incs/Config.hpp"

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

int		 	Request::base64_decode(const char * text, char * dst, int numBytes)
{
    const char* cp;
    int space_idx = 0, phase;
    int d, prev_d = 0;
    char c;
    space_idx = 0;
    phase = 0;
    for (cp = text; *cp != '\0'; ++cp) {
        d = Config::decodeMimeBase64[(int)*cp];
        if (d != -1) {
            switch (phase) {
            case 0:
                ++phase;
                break;
            case 1:
                c = ((prev_d << 2) | ((d & 0x30) >> 4));
                if (space_idx < numBytes)
                    dst[space_idx++] = c;
                ++phase;
                break;
            case 2:
                c = (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
                if (space_idx < numBytes)
                    dst[space_idx++] = c;
                ++phase;
                break;
            case 3:
                c = (((prev_d & 0x03) << 6) | d);
                if (space_idx < numBytes)
                    dst[space_idx++] = c;
                phase = 0;
                break;
            }
            prev_d = d;
        }
    }
    return space_idx;
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
    Location &loc = m_client->getServer()->getPerfectLocation(this->m_reqlocation);
	m_client->getResponse().setLocation(&loc);

    if (isValidAuthHeader(loc) == false)
	{
        m_client->setCStatus(RESPONSE_MAKING);
        // this->m_client->getResponse().makeErrorResponse(401);
        return (false);
    }
    if (isValidMethod(loc) == false)
    {
        m_client->setCStatus(RESPONSE_MAKING);
        // this->client->getResponse().makeErrorResponse(405);
        return (false);
    }
    if (isValidRequestMaxBodySize(loc) == false)
    {
        m_client->setCStatus(RESPONSE_MAKING);
        // this->client->getResponse().makeErrorResponse(413);
        return (false);
    }

    //GET /index.html http1.1
	std::string resource_path = loc.getRoot() + this->m_reqlocation.substr(loc.getUri().size());
    m_client->getResponse().setResourcePath(resource_path);

    for (std::map<std::string, std::string>::iterator iter = loc.getCgi().begin(); iter != loc.getCgi().end(); iter++)
    {
        if (resource_path.find(iter->first) != std::string::npos) // cgi_extention 표현을 찾았다면
        {
            this->m_client->getResponse().setCgi(iter->first);
            break;
        }
    }
    if (loc.getReturnNum() != -1)
        this->m_client->getResponse().setReturn(true);

    setRequestStatus(HEADER_PARSING);
    return true;
}

bool	Request::isValidAuthHeader(Location &loc)
{
	if (loc.getAuthKey() != "")
	{
		char result[200];
		memset(result, 0, 200);

		if (this->m_headersMap.find("Authorization") == this->m_headersMap.end())  // auth key 헤더가 아예 안들어왔다.
		{
			return (false);
		}
		else
		{
			size_t idx = this->m_headersMap["Authorization"].find_first_of(' ');
			std::string secret = this->m_headersMap["Authorization"].substr(idx + 1);
			base64_decode(secret.c_str(), result, secret.size());
			if (std::string(result) != loc.getAuthKey()) // 키가 맞지 않는다.
				return (false);
		}
	}
	return (true);
}

bool	Request::isValidMethod(Location &loc)
{
	bool isAllowCheckOkay = false;
	for (std::vector<std::string>::iterator iter = loc.getAllowMethods().begin(); iter != loc.getAllowMethods().end(); iter++)
	{
		if (this->m_method == *iter)
		{
			isAllowCheckOkay = true;
			break ;
		}
	}
	if (isAllowCheckOkay != true) // allow method check 가 안되었다. -> 405
		return (false);
	return (true);
}

bool	Request::isValidRequestMaxBodySize(Location &loc)
{
	if (this->m_body.size() > (size_t)(loc.getMaxBodySize()))
		return (false);
	return (true);
}