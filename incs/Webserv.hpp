#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

# define MAX_FD_SIZE 1024

class Server;
class Location;

class Webserv 
{
	private:
		int m_kq;
		struct kevent m_return_events[1024];
		std::vector<FdBase *> m_fd_pool;

	public:
		Webserv();
		Webserv(const Webserv &other);
		virtual ~Webserv();
		Webserv &operator=(const Webserv &other);
};

#endif