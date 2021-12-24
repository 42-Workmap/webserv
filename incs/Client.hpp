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
			RESPONSE_MAKING,
}			e_c_status;


class Client : public FdBase
{
	private:
		Request m_request;
		Response m_response;
		e_c_status m_c_status;
		struct timeval m_last_time;

		Server *m_server;

	public:
		Client();
		Client(Server *server, int c_fd);
		virtual ~Client();
		Client(const Client &other);
		Client &operator=(const Client &other);

		Request &getRequest(void);
		Response &getResponse(void);
		const e_c_status getCStatus(void) const;
		const struct timeval getLastTime(void) const;
		Server* getServer(void);

		void setLastTime(struct timeval last_time);
		void setCStatus(e_c_status c_status);

		void appendOrigin(std::string newstr);
		bool parseRequest();
};

#endif