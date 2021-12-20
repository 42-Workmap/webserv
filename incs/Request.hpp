#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "Client.hpp"
#include "Response.hpp"

class Response;
class Client;

class Request 
{
	private:
		std::string m_origin;
		std::string m_body;

		std::string m_method;
		std::string m_reqlocation;
		std::string m_httpver;
		std::map<std::string, std::string> m_headersMap;

		std::string m_request_status;

		Client *m_client;
		size_t m_remain_body_value;
		
	public:
		Request();
		virtual ~Request();
		Request(const Request &other);
		Request &operator=(const Request &other);
};

#endif