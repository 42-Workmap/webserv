#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>

// #include "Client.hpp"
#include "Location.hpp"

class Location;
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

		//get
		bool getReturn();
		bool getDisconnect();
		Client* getClient();
		std::string& getOrigin();
		std::string getResourcePath();
		Location* getLocation();
		std::string getCgiExtention();
		size_t getWriteIdx();
		int getFdRead();
		int getFdWrite();

		//set
		void setReturn(bool);
		void setDisconnect(bool);
		void setClient(Client* client);
		void setOrigin(std::string origin);
		void setResourcePath(std::string path);
		void setLocation(Location *location);
		void setCgi(std::string cgi);
		void setWriteIdx(size_t idx);
		void setFdRead(int fd);
		void setFdWrite(int fd);
};

#endif