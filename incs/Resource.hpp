#ifndef RESOURCE_HPP
# define RESOURCE_HPP

#include <string>
#include "FdBase.hpp"

class Client;

typedef enum t_resource_types
{
    FD_TO_RAWDATA,
    RAWDATA_TO_FD

}           e_resource_type;

typedef enum			t_nextcall
{
	MAKING_ERROR_RESPONSE,
	MAKING_RESPONSE
}						e_nextcall;

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
        Resource();
        virtual ~Resource();
        Resource(Resource & rsc);
        Resource& operator=(Resource& rsc);

};


#endif