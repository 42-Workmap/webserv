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
class Response;
class Request;

class Client : public FdBase
{
	private:
		Request m_request;
		Response m_response;
		std::string m_c_status;
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
		const std::string getCStatus(void) const;
		const struct timeval getLastTime(void) const;
		Server* getServer(void);

		void setLastTime(struct timeval last_time);
		void setCStatus(std::string c_status);

};

#endif