#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>

#include "Libft.hpp"

class Client;
class Location;

typedef enum t_req_status
{
	HEADER_PARSING, 
	BODY_PARSING,
	CHUCKED,
	CHUCKED_BODY,
	CONTENT_BODY
} 			e_req_status;

class Request 
{
	private:
		std::string m_origin;
		std::string m_body;

		std::string m_method;
		std::string m_reqlocation;
		std::string m_httpver;
		std::map<std::string, std::string> m_headersMap;

		e_req_status m_request_status;

		Client *m_client;
		size_t m_remain_body_value;	
		int base64_decode(const char * text, char * dst, int numBytes);

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
		e_req_status& getRequestStatus();
		Client* getClient();
		size_t getRemainBodyValue();

		//set
		void setOrigin(std::string);
		void setBody(std::string);
		void setMethod(std::string);
		void setReqLocation(std::string);
		void setHttpver(std::string);
		// void setHeadersMap();
		void setRequestStatus(e_req_status);
		void setClient(Client* client);
		void setRemainBodyValue(size_t len);

		bool makeHeader(void);
		bool makeBody(void);
		bool checkValidRequest(std::string fin);
		bool isValidAuthHeader(Location &loc);
		bool isValidMethod(Location &loc);
		bool isValidRequestMaxBodySize(Location &loc);
};

#endif