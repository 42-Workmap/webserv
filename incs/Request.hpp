#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>

// #include "Client.hpp"

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

		//get
		std::string& getOrigin();
		std::string& getBody();
		std::string& getMethod();
		std::string& getReqLocation();
		std::string& getHttpVer();
		std::map<std::string, std::string>& getHeadersMap();
		std::string& getRequestStatus();
		Client* getClient();
		size_t getRemainBodyValue();

		//set
		void setOrigin(std::string);
		void setBody(std::string);
		void setMethod(std::string);
		void setReqLocation(std::string);
		void setHttpver(std::string);
		// void setHeadersMap();
		void setRequestStatus(std::string);
		void setClient(Client* client);
		void setRemainBodyValue(size_t len);

		bool makeHeader();
};

#endif