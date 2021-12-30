#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

// #include "Client.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "Resource.hpp"


class Location;
class Client;


class Response 
{
	private:
		bool m_return;
		bool m_disconnect;
		Client *m_client;

		std::string m_message;
		std::string m_resource_path;
		Location *m_location;

		std::string m_cgi_extention;
		size_t m_write_idx;
		std::list<Resource *> m_resourceList;

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
		std::string& getMessage();
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
		void setMessage(std::string message);
		void setResourcePath(std::string path);
		void setLocation(Location *location);
		void setCgi(std::string cgi);
		void setWriteIdx(size_t idx);
		void setFdRead(int fd);
		void setFdWrite(int fd);
		void setResource(int res_fd, e_resource_type type, e_nextcall ctype, int errornum = -1);

		void makeResponse();
		void makeGetResponse();
		void makeErrorResponse(int err);
		void makeAutoIndexPage(void);

		void addStatusLine(int err);
		void addDate();
		void addContentLanguage();
		void addContentType(std::string type);
		void addContentLength(int size);
		void addEmptyLine();

		bool isDirectory(std::string path);
		bool isExist(std::string path);

};

#endif