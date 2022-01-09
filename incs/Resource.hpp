#ifndef RESOURCE_HPP
# define RESOURCE_HPP

#include <string>
#include <unistd.h>
#include <iostream>
#include "FdBase.hpp"

class Client;

typedef enum t_resource_types
{
    READ_RESOURCE, // FD_TO_RAW_DATA
    WRITE_RESOURCE // RAWDATA_TO_FD
}           e_resource_type;

typedef enum			t_nextcall
{
	MAKING_ERROR_RESPONSE,
	MAKING_RESPONSE
}						e_nextcall;

typedef enum    t_rsc_status
{
    READY, 
    NOT_YET, 
    CGI_CRASH
}               e_rsc_status;

class Resource : public FdBase
{
    private:
        std::string &m_raw_data;  // response의 m_body
        std::string m_unlink_path;
        Client *m_client;
        e_resource_type m_type;
        e_nextcall m_next;
        int m_response_error_num;
        size_t m_write_index;

        int m_pid;
        bool m_is_seeked;

    public:
        // Resource();  // 기본 생성자를 쓸 일이 없음 일단 주석~
        Resource(int fd, std::string& response_message, Client* clnt, e_resource_type type, e_nextcall nxt, int err);
        virtual ~Resource();
        Resource(Resource & rsc);
        Resource& operator=(Resource& rsc);

        // get
        std::string& getRawData();
        std::string getUnlinkPath();
        Client* getClient();
        e_resource_type getResourceType();
        e_nextcall getNextCall();
        int getResponseErrorNum();
        size_t getWriteIndex();
        int getPid();
        bool getIsSeeked();

        //set
        void setUnlinkPath(std::string path);
        void setClient(Client * clnt);
        void setResourceType(e_resource_type type);
        void setNext(e_nextcall next);
        void setResponseErrorNum(int err_num);
        void setWriteIndex(size_t idx);
        void setPid(int pid);
        void setIsSeeked(bool seeked);

        e_rsc_status isReady(void);
        void doNext(void);
};


#endif