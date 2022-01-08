#include "../incs/Client.hpp"
#include "../incs/Response.hpp"

Response::Response()
{
    this->m_return = false;
    this->m_disconnect = false;
    this->m_message.clear();
    this->m_resource_path.clear();
    this->m_location = NULL;
    this->m_cgi_extension.clear();
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

    m_cgi_extension = other.m_cgi_extension;
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

std::string Response::getCgiExtension()
{
    return m_cgi_extension;
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

std::list<Resource *>& Response::getResourceList()
{
    return m_resourceList;
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
    m_cgi_extension = cgi;
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

char **Response::makeCgiEnv(void)
{
    char **ret;

    std::map<std::string, std::string> cgi_map;
    size_t idx;
    std::map<std::string, std::string> &headersMap = m_client->getRequest().getHeadersMap();

    std::string &reqlocation = getClient()->getRequest().getReqLocation(); //"/"
    std::string path_info = reqlocation.substr(reqlocation.find(m_cgi_extension) + m_cgi_extension.size()); // "/abc.cgi" ""?
    std::string path_translated = m_resource_path.substr(0, m_resource_path.find(m_cgi_extension) + m_cgi_extension.size());
    std::string query_string; //?~

    idx = path_info.find('?');
    if (idx != std::string::npos)
    {
        query_string = path_info.substr(idx+1);
        path_info = path_info.substr(0, idx); // ?없앰
    }
    if (path_info == "")
        path_info += '/';
    
    if (headersMap.count("Authorization") == 1)
    {
        idx = headersMap["Authorization"].find(' ');
        cgi_map["AUTH_TYPE"] = headersMap["Authorization"].substr(0, idx);
        cgi_map["REMOTE_USER"] = headersMap["Authorization"].substr(idx+1);
        cgi_map["REMOTE_IDENT"] = headersMap["Authorization"].substr(idx+1);
    }
    cgi_map["CONTENT_LENGTH"] = std::to_string(getClient()->getRequest().getBody().size());
    if (headersMap.count("Content-Type") == 1)
    {
        cgi_map["CONTENT_TYPE"] = headersMap["Content-Type"];
    }
    cgi_map["GATEWAY_INTERFACE"] = "CGI/1.1";
    cgi_map["PATH_INFO"] = path_info;
    cgi_map["PATH_TRANSLATED"] = path_translated;
    cgi_map["QUERY_STRING"] = query_string;
    cgi_map["REMOTE_ADDR"] = m_client->getServer()->getIp();
    cgi_map["REQUEST_METHOD"] = m_client->getRequest().getMethod();
    if (m_cgi_extension == ".bla")
        cgi_map["REQUEST_URI"] = path_info; // query까지
    else
        cgi_map["REQUEST_URI"] = m_client->getRequest().getReqLocation();
    cgi_map["SCRIPT_NAME"] = m_location->getCgi()[m_cgi_extension];
    cgi_map["SERVER_NAME"] = m_client->getServer()->getServerName();
    cgi_map["SERVER_PORT"] = m_client->getServer()->getPort();
    cgi_map["SERVER_PROTOCOL"] = "HTTP/1.1";
    cgi_map["SERVER_SOFTWARE"] = "nginx";
    // cgi_map["REDIRECT_STATUS"]
    cgi_map["SCRIPT_FILENAME"] = path_translated;

    for (std::map<std::string, std::string>::iterator it = cgi_map.begin(); it != cgi_map.end(); it++)
        cgi_map["HTTP_" + it->first] = it->second;
    if (!(ret = (char **)malloc(sizeof(char *) * (cgi_map.size() + 1))))
        return (NULL);
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = cgi_map.begin(); it != cgi_map.end(); it++)
    {
        ret[i] = strdup((it->first + "=" + it->second).c_str());
        i++;
    }
    ret[i] = NULL;
    return (ret);
}

void Response::makeCgiResponse(void)
{
    if (m_client->getCStatus() == MAKE_RESPONSE)
    {
        std::cout << "got into MakeCGI()" << std::endl;
        int fds[2];

        if ((pipe(fds)) == -1)
            return (makeErrorResponse(500));
        m_fd_read = fds[0];
        m_fd_write = fds[1];

        fcntl(fds[1], F_SETFL, O_NONBLOCK);
        setResource(fds[1], WRITE_RESOURCE, MAKING_RESPONSE, -1);

        mkdir("./temp", 0777);
        std::string temp_file_name = "./temp/tempfile_" + std::to_string(fds[0]);
        int fd_temp = open(temp_file_name.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);

        if (fd_temp == -1)
            return (makeErrorResponse(500));
        int pid = fork();
        if (pid == 0) // 자식이면  
        {  
            close(m_fd_write); // stdout X
            dup2(m_fd_read, 0); //stdin -> fd_read 
            dup2(fd_temp, 1); // stdout -> tempfile 에 적겠다 

            char *args[3];
            std::string uriuntilparam = m_resource_path.substr(0, m_resource_path.find(m_cgi_extension) + m_cgi_extension.size());
            args[0] = strdup(m_location->getCgi()[m_cgi_extension].c_str());
            args[1] = strdup(uriuntilparam.c_str());
            args[2] = 0;

            char **cgi_env = makeCgiEnv();

            int ret;
            ret = execve(args[0], args, cgi_env);
            std::cout << "writing from CGI() " << std::endl;
            close(m_fd_read);
            exit(ret);
        }
        else if (pid < 0)
            return (makeErrorResponse(500));
        else 
        {
            std::cout << "pid != 0 cgi()" << std::endl;
            Resource *res = new Resource(fd_temp, m_message, m_client, READ_RESOURCE, MAKING_RESPONSE, -1);
            res->setPid(pid);
            res->setUnlinkPath(temp_file_name);
            m_resourceList.push_back(res);
            Config::getConfig()->getWebserv()->addFdPool(res);
            Config::getConfig()->getWebserv()->change_events(Config::getConfig()->getWebserv()->getChangeList(), \
                                    fd_temp, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

        }
    }
    else if (m_client->getCStatus() == FILE_READ_DONE)
    {
        close(m_fd_read);
        std::cout << "CGI" << m_message << std::endl; // 나중을 위한 print
        std::string read_result = m_message.substr();

        m_message.clear(); 
        size_t status_idx1 = read_result.find("Status :") + 8;
        size_t status_idx2 = read_result.find("\r\n", status_idx1);
        m_message += "HTTP/1.1" + read_result.substr(status_idx1, status_idx2 - status_idx1) + "\r\n";
        // newhader HTTP/1.1 200 OK 
        addDate();
        addContentLanguage();
        size_t body_size = read_result.substr(read_result.find("\r\n\r\n") + 4).size();
        addContentLength(body_size);
        m_message += read_result;
        m_client->setCStatus(MAKE_RESPONSE_DONE);
    }
}

void Response::makeGetResponse()
{
    int fd;
    struct stat sb;
    size_t idx;

    if (isDirectory(m_resource_path))
    {
        if (*(--m_resource_path.end()) != '/')
        {
            m_resource_path += '/';
        }
        bool is_exist = false;
        std::string pathwithfile;
        for (std::vector<std::string>::iterator it = m_location->getIndexs().begin();
             it != m_location->getIndexs().end(); it++)
        {
            pathwithfile = m_resource_path + (*it);
            is_exist = isExist(pathwithfile);
            if (is_exist == true)
                break;
        }
        if (is_exist == false && m_location->getAutoIndex() == true)
            return (makeAutoIndexPage());
        m_resource_path = pathwithfile;
    }
    if (!isExist(m_resource_path)) // not found
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
    if (idx == std::string::npos)
        addContentType(".arc");
    else
        addContentType(m_resource_path.substr(idx));

    addContentLength((int)sb.st_size);
    addEmptyLine();
    std::cout << m_message << std::endl; // test header
    setResource(fd, READ_RESOURCE, MAKING_RESPONSE);
    // setReadResource(fd, READ_FD, MAKING_RESPONSE);
}

void Response::makePostResponse(void)
{
    if (isDirectory(m_resource_path))
    {
        makeErrorResponse(400);
        std::cout << "directory error" << std::endl;
        return;
    }

    int fd;
    if (isExist(m_resource_path))
    {
        if ((fd = open(m_resource_path.c_str(), O_WRONLY | O_APPEND, 0777)) < 0)
        {
            makeErrorResponse(500);
            std::cout << "file create error" << std::endl;
            return;
        }
        addStatusLine(204);
    }
    else
    {
        if ((fd = open(m_resource_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777)) < 0)
        {
            makeErrorResponse(500);
            std::cout << "file open error" << std::endl;
            return;
        }
        addStatusLine(201);
    }
    addDate();
    struct stat sb;
    if (fstat(fd, &sb) < 0)
        return (makeErrorResponse(500));
    addContentLength(0);
    addEmptyLine();
    setResource(fd, WRITE_RESOURCE, MAKING_RESPONSE);
    m_client->setCStatus(FILE_WRITING);
}

void Response::makeRedirection(void)
{
    m_message.clear();
    addStatusLine(m_location->getReturnNum());
    addDate();
    addServer();
    addLocation(m_location->getReturnUrl());
    addEmptyLine();
    
    m_client->setCStatus(MAKE_RESPONSE_DONE);
    setDisconnect(true);
    return ;
}

void Response::makeDeleteResponse(void)
{
    std::cout << "Delete() func here" << std::endl;
    m_message.clear();
    if (!isExist(m_resource_path))
        makeErrorResponse(404);
    else
    {
        if (isDirectory(m_resource_path))
            makeErrorResponse(403);
        else
        {
            int del = unlink(m_resource_path.c_str());
            if (del < 0)
                makeErrorResponse(403);
            else
            {
                addStatusLine(204);
                addDate();
                addServer();
                addContentLength(0);
                addEmptyLine();
            }
        }
    }
    m_client->setCStatus(MAKE_RESPONSE_DONE);
}

void Response::makeErrorResponse(int errorcode)
{
    m_message.clear();
    std::cout << "error" << errorcode << std::endl;
    addStatusLine(errorcode);
    addDate();
    addContentLanguage();
    addServer();
    addContentType(".html");
    if (errorcode == 401)
        addWWWAuthenticate();
    if (errorcode == 405)
        addAllowMethod();
    if (m_location->getErrorPages().count(errorcode) == 0)  // default 에러 페이지 없으면
        return(addErrorBody(errorcode));

    std::string resource_path = getLocation()->getErrorPages()[errorcode];
    struct stat sb;
    int fd;
    if ((fd = open(resource_path.c_str(), O_RDONLY)) < 0)
        return (addErrorBody(errorcode));
    else
    {
        if (fstat(fd, &sb) < 0)
            return (addErrorBody(errorcode));
        else
        {
            addContentLength(int(sb.st_size));
            addEmptyLine();
            setResource(fd, READ_RESOURCE, MAKING_RESPONSE);
        }
    }
}

void Response::makeAutoIndexPage(void)
{
    m_message.clear();
}

void Response::setResource(int res_fd, e_resource_type type, e_nextcall ctype, int errornum)
{
    Resource* res;
    
    Webserv* webserv = Config::getConfig()->getWebserv();

    if (type == WRITE_RESOURCE)
    {
       res = new Resource(res_fd, m_client->getRequest().getBody(), m_client, WRITE_RESOURCE, ctype, errornum);
       m_resourceList.push_back(res);
       webserv->addFdPool(dynamic_cast<FdBase*>(res));
       webserv->change_events(webserv->getChangeList(), res_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    }
    else if (type == READ_RESOURCE)
    {
        res = new Resource(res_fd, m_message, m_client, READ_RESOURCE, ctype, errornum);
        m_resourceList.push_back(res);
        webserv->addFdPool(dynamic_cast<FdBase *>(res));
        webserv->change_events(webserv->getChangeList(), res_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);  
    }
}

void Response::initResponse()
{
    m_return = false;
    m_disconnect = false;
    m_message.clear();
    m_resource_path.clear();
    m_location = NULL;
    m_cgi_extension.clear();
    m_write_idx = 0;
    m_resourceList.clear();
    m_fd_read = -1;
    m_fd_write = -1;
}

