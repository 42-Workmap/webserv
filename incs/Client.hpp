#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "FdBase.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Server.hpp"

class Server;

typedef enum t_c_status
{
			REQUEST_RECEIVING,
			MAKE_RESPONSE,
			FILE_READING,
			FILE_READ_DONE,
			FILE_WRITING,
			FILE_WRITE_DONE,
			MAKE_RESPONSE_DONE
}			e_c_status;


class Client : public FdBase
{
	private:
		Request m_request;
		Response m_response;
		e_c_status m_c_status;
		unsigned long m_last_time;

		Server *m_server;

	public:
		Client();
		Client(Server *server, int c_fd);
		virtual ~Client();
		Client(const Client &other);
		Client &operator=(const Client &other);

		Request &getRequest(void);
		Response &getResponse(void);
		e_c_status &getCStatus(void);
		unsigned long &getLastTime(void);
		Server* getServer(void);

		void setLastTime(unsigned long last_time);
		void setCStatus(e_c_status c_status);

		void appendOrigin(std::string newstr);
		bool parseRequest();
		void makeResponse();
		void initRequestandResponse();
};

#endif