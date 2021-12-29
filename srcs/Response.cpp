#include "../incs/Client.hpp"
#include "../incs/Response.hpp"

Response::Response()
{
    this->m_return = false;
    this->m_disconnect = false;
    this->m_message.clear();
    this->m_resource_path.clear();
    this->m_location = NULL;
    this->m_cgi_extention.clear();
    this->m_write_idx = 0;
    this->m_resourceList.clear();
    this->m_fd_read = -1;
    this->m_fd_write = -1;
}

Response::~Response()
{
    if (this->m_fd_read != -1)
        close(this->m_fd_read);
    if (this->m_fd_write != -1)
        close(this->m_fd_write);
    // for문 필요 
}

Response::Response(const Response &other)
{
    *this = other;
}

Response& Response::operator=(const Response &other)
{
    m_return = other.m_return;
    m_disconnect = other.m_disconnect;
    m_client = other.m_client;
    m_message = other.m_message;
    m_resource_path = other.m_resource_path;
    m_location = other.m_location;

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

std::string& Response::getMessage()
{
    return m_message;
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

void Response::setMessage(std::string message)
{
    m_message = message;
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

void Response::makeGetResponse()
{
    if (m_client->getCStatus() == MAKE_RESPONSE)
    {
        int fd;
        struct stat sb;
        size_t idx;

        if(isDirectory(m_resource_path))
        {
            if (*(--m_resource_path.end()) != '/')
            {
                m_resource_path += '/';
            }
            bool is_exist = false;
            std::string pathwithfile;
            for (std::vector<std::string>::iterator it = m_location->getIndexs().begin(); \
                it != m_location->getIndexs().end(); it++)
                {
                    pathwithfile = m_resource_path + (*it);
                    is_exist = isExist(pathwithfile);
                    if (is_exist == true)
                        break ;
                }
            if (is_exist == false && m_location->getAutoIndex() == true)
                return (makeAutoIndexPage());
            m_resource_path = pathwithfile;
        }
        if (!isExist(m_resource_path))   // not found
            return (makeErrorResponse(404));
        if ((fd = open(m_resource_path.c_str(), O_RDONLY)) < 0)
            return (makeErrorResponse(500));
        if (fstat(fd, &sb) < 0)
            return (makeErrorResponse(500));

        // 문제가 없을 경우 헤더 만들기 시작 
        addStatusLine(200);
        addDate();
        addContentLanguage();
        idx = m_resource_path.find_first_of('/');
        idx = m_resource_path.find_first_of('.', idx);
        if(idx == std::string::npos)
            addContentType(".arc");
        else
            addContentType(m_resource_path.substr(idx));

        addContentLength((int)sb.st_size);
        addEmptyLine();
        std::cout << m_message << std::endl; // test header 
        // setResource(fd, FD_TO_RAWDATA, MAKING_RESPONSE);
        // setReadResource(fd, READ_FD, MAKING_RESPONSE);
    }
    else if (m_client->getCStatus() == FILE_READ_DONE)
        m_client->setCStatus(MAKE_RESPONSE_DONE);
    return ;
}

void Response::makeErrorResponse(int errorcode)
{
    m_message.clear();

    addStatusLine(errorcode);
}

void Response::makeAutoIndexPage(void)
{
    m_message.clear();
}

// void Response::setReadResource(int fd, e_resource_type rtype, e_nextcall ncall, int err)
// {
//     Resource *res;
//     res = new Resource(fd, this->m_message, this->m_client, READ_RESOURCE, ncall, err);
//     this->m_resourcesList.push_back(res);
//     Config::getConfig()->getWebserv()->insertFdPools(res);
// }