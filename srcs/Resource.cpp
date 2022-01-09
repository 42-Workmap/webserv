#include "../incs/Resource.hpp"

// Resource::Resource()
// {

// }

Resource::Resource(int fd, std::string& response_message, Client* clnt, e_resource_type type, e_nextcall nxt, int err) : m_raw_data(response_message), m_client(clnt), m_type(type), m_next(nxt), m_response_error_num(err)
{
    m_fd = fd;
    m_fd_type = FD_RESOURCE;
    m_pid = -1;
    m_is_seeked = false;
    m_write_index = 0;
    m_unlink_path.clear();
    m_next = nxt;
}


Resource::~Resource()
{

}

// 멤버변수 getRawData가 참조라서
Resource::Resource(Resource & rsc) : m_raw_data(rsc.m_raw_data)
{
    *this = rsc;
}

Resource& Resource::operator=(Resource& rsc)
{
    m_fd = rsc.m_fd;
    m_fd_type = rsc.m_fd_type;
    m_pid = rsc.m_pid;
    m_is_seeked = rsc.m_is_seeked;
    m_next = rsc.m_next;
    m_raw_data = rsc.m_raw_data;
    m_response_error_num = rsc.m_response_error_num;
    m_type = rsc.m_type;
    m_unlink_path = rsc.m_unlink_path;
    m_write_index = rsc.m_write_index;
    return *this;
}

std::string& Resource::getRawData()
{
    return m_raw_data;
}

std::string Resource::getUnlinkPath()
{
    return m_unlink_path;
}

Client* Resource::getClient()
{
    return m_client;
}

e_resource_type Resource::getResourceType()
{
    return m_type;
}

e_nextcall Resource::getNextCall()
{
    return m_next;
}

int Resource::getResponseErrorNum()
{
    return m_response_error_num;
}

size_t Resource::getWriteIndex()
{
    return m_write_index;
}

int Resource::getPid()
{
    return m_pid;
}

bool Resource::getIsSeeked()
{
    return m_is_seeked;
}

void Resource::setUnlinkPath(std::string path)
{
    m_unlink_path = path;
}
void Resource::setClient(Client * clnt)
{
    m_client = clnt;
}
void Resource::setResourceType(e_resource_type type)
{
    m_type = type;
}
void Resource::setNext(e_nextcall next)
{
    m_next = next;
}
void Resource::setResponseErrorNum(int err_num)
{
    m_response_error_num = err_num;
}

void Resource::setWriteIndex(size_t idx)
{
    m_write_index = idx;
}
void Resource::setPid(int pid)
{
    m_pid = pid;
}

void Resource::setIsSeeked(bool seeked)
{
    m_is_seeked = seeked;
}

e_rsc_status Resource::isReady(void)
{
    int status;

    if (m_pid == -1)
        return (READY);
    else 
    {
        std::cout << "isReady()" << std::endl; // 절대 여기로 들어오지 않는다... 
        if (waitpid(m_pid, &status, WNOHANG) == 0)
        {
            return (NOT_YET);
        }
        else
        {
            if (WIFEXITED(status) == 0)
                return (CGI_CRASH);
            else
            {
                if (m_is_seeked == false)
                {
                    lseek(m_fd, 0, SEEK_SET);
                    m_is_seeked = true;
                }
                std::cout << "READY" << std::endl;
                return (READY);
            }
        }
    }
    
}