#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "Client.hpp"
#include "Response.hpp"

class Response;
class Client;

class Response 
{
	private:
		bool m_return;
		bool m_disconnect;
		Client *m_client;

		std::string m_origin;
		std::string m_resource_path;
		Location *m_location;

		std::string m_cgi_extention;
		size_t m_write_idx;
		// std::list<Resource *> m_resourceList;

		int m_fd_read;
		int m_fd_write;

	public:
		Response();
		virtual ~Response();
		Response(const Response &other);
		Response &operator=(const Response &other);
};

#endif