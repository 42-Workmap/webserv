#ifndef WEBSERV_HPP
# define WEBSERV_HPP

class Webserv 
{
	private:
		int kq;
		struct kevent return_events[1024];
		std::vector<FdBase *> m_fd_pool;
		std::map<std::string, Server> m_ServerMap;
		std::map<std::string, std::string> m_mimetype;


	public:
		Webserv();
		Webserv(const Webserv &other);
		virtual ~Webserv();
		Webserv &operator=(const Webserv &other);
}

#endif