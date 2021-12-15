#ifndef SERVER_HPP
# define SERVER_HPP

#include "FdBase.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <map>

class Server : public FdBase
{
	private: 
		std::string m_server_name;
		std::string m_ip;
		std::string m_port;
		// std::map<std::string, Location *>LocationMap;
		// std::vector<Client *> ClientVec;

	public:
		Server();
		Server(const Server &other);
		virtual ~Server();
		Server &operator=(const Server &other);

		void setServerName(std::string server_name);
		void setIp(std::string ip);
		void setPort(std::string port);

		std::string getServerName(void) const;
		std::string getIp(void) const;
		std::string getPort(void) const;


};

#endif