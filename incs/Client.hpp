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

class FDBase;
class Server;
class Response;
class Request;

class Client : public FDBase
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

};

#endif