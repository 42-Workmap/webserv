#include "../incs/Client.hpp"
#include "../incs/Response.hpp"

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
    cgi_map["SCRIPT_EXEC"] = m_location->getCgi()[m_cgi_extension];
    cgi_map["SERVER_NAME"] = m_client->getServer()->getServerName();
    cgi_map["SERVER_PORT"] = m_client->getServer()->getPort();
    cgi_map["SERVER_PROTOCOL"] = "HTTP/1.1";
    cgi_map["SERVER_SOFTWARE"] = "nginx";
    cgi_map["SCRIPT_FILENAME"] = path_translated;

    cgi_map["REDIRECT_STATUS"]="200";
    cgi_map["SCRIPT_NAME"] = m_location->getCgi()[m_cgi_extension];

    for (std::map<std::string, std::string>::iterator it = headersMap.begin(); it != headersMap.end(); it++)
        cgi_map["HTTP_" + it->first] = it->second;
    if (!(ret = (char **)malloc(sizeof(char *) * (cgi_map.size() + 1))))
        return (NULL);
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = cgi_map.begin(); it != cgi_map.end(); it++)
    {
        ret[i] = strdup((it->first + "=" + it->second).c_str());
        std::cout << ret[i] << std::endl;
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
        int read_fds[2];
        int write_fds[2];

        if ((pipe(read_fds)) == -1)
            return (makeErrorResponse(500));
        if ((pipe(write_fds)) == -1)
            return (makeErrorResponse(500));

        int pid = fork();
        if (pid == 0) // 자식이면  
        {  
            char *args[3];
            std::string uriuntilparam = m_resource_path.substr(0, m_resource_path.find(m_cgi_extension) + m_cgi_extension.size());
            args[0] = strdup(m_location->getCgi()[m_cgi_extension].c_str());
            args[1] = strdup(uriuntilparam.c_str());
            args[2] = 0;
            char **cgi_env = makeCgiEnv();

            dup2(write_fds[0], STDIN_FILENO);
            dup2(read_fds[1], STDOUT_FILENO);
            close(write_fds[0]);
            close(write_fds[1]);
            close(read_fds[0]);
            close(read_fds[1]);

            int ret;
            ret = execve(args[0], args, cgi_env);
            exit(ret);
        }
        else if (pid < 0)
            return (makeErrorResponse(500));
        else 
        {
            close(write_fds[0]);
            close(read_fds[1]);

            m_fd_read = read_fds[0];
            m_fd_write = write_fds[1];

            fcntl(write_fds[1], F_SETFL, O_NONBLOCK);
            setResource(write_fds[1], WRITE_RESOURCE, MAKING_RESPONSE, -1);

            std::cout << "pid != 0 cgi()" << std::endl;
            Resource *res = new Resource(m_fd_read, m_message, m_client, READ_RESOURCE, MAKING_RESPONSE, -1);
            res->setPid(pid);
            m_resourceList.push_back(res);
            Config::getConfig()->getWebserv()->addFdPool(res);
            Config::getConfig()->getWebserv()->change_events(Config::getConfig()->getWebserv()->getChangeList(), \
                                    m_fd_read, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
            m_client->setCStatus(FILE_WRITING);

        }
    }
    else if (m_client->getCStatus() == FILE_READ_DONE)
    {
        close(m_fd_read);
        std::string read_result = m_message.substr();

        m_message.clear(); 
        size_t status_idx1 = read_result.find("Status: ") + 8;
        size_t status_idx2 = read_result.find("\r\n", status_idx1);
        m_message += "HTTP/1.1 " + read_result.substr(status_idx1, status_idx2 - status_idx1) + "\r\n";
        addDate();
        addContentLanguage();
        size_t body_size = read_result.substr(read_result.find("\r\n\r\n") + 4).size();
        addContentLength(body_size);
        m_message += read_result;
        m_client->setCStatus(MAKE_RESPONSE_DONE);
    }
}