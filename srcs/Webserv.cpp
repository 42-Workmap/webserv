#include "../incs/Webserv.hpp"

Webserv::Webserv()
{
	this->m_fd_pool.resize(MAX_FD_SIZE, NULL);
	this->m_timeout = 6000000;
	std::cout << "Webserv constructor called" << std::endl;
}

Webserv::Webserv(const Webserv &other)
{
	*this = other;
}

Webserv::~Webserv()
{
	std::cout << "Webserv destructor called" << std::endl;
	// clean_everything();
}

Webserv &Webserv::operator=(const Webserv &other)
{
	this->m_kq = other.m_kq;
	this->m_fd_pool = other.m_fd_pool;
	for (int i = 0; i < MAX_FD_SIZE; i++)
	{
		this->m_return_events[i] = other.m_return_events[i];
	}
	return (*this);
}

static void error_handling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void Webserv::change_events(std::vector<struct kevent> &change_list, uintptr_t ident, int16_t filter,
							uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}

void Webserv::startServer()
{
	Config* config = Config::getConfig();
	// std::cout << config->getServerMap().begin()->second;
	if((m_kq = kqueue()) == -1)
	{
		error_handling("kqueue() error");
	}

	for (std::map<std::string, Server>::iterator it = config->getServerMap().begin(); it != config->getServerMap().end(); it++)
	{
		int serv_sock;
		struct sockaddr_in serv_addr;

		serv_sock = socket(PF_INET, SOCK_STREAM, 0); /* 서버 소켓 생성 */
		it->second.setFd(serv_sock);
		if (serv_sock == -1)
			error_handling("socket() error");
		int opt = 1;
		setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = inet_addr(it->second.getIp().c_str());
		serv_addr.sin_port = htons(std::atoi(it->second.getPort().c_str()));

		/* 소켓에 주소 할당 */
		if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
			error_handling("bind() error");
		if (listen(serv_sock, 1024) == -1) /* 연결 요청 대기 상태로 진입 */
			error_handling("listen() error");
		std::cout << it->second.getIp() << ":" << it->second.getPort() << " server on"<< "\n";
		fcntl(serv_sock, F_SETFL, O_NONBLOCK);
		change_events(m_change_list, serv_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		m_fd_pool[serv_sock] = &(it->second);
	}

	testServer();
}

void Webserv::testServer(void)
{
	int new_events;
	struct kevent* curr_event;
	while (1)
	{
		new_events = kevent(m_kq, &m_change_list[0], m_change_list.size(), m_return_events, 1024, NULL);
		if (new_events == -1)
		{
			error_handling("kevent() error\n");
		}

		m_change_list.clear();
		for (int i = 0; i < new_events; i++)
		{
			curr_event = &m_return_events[i];
			if (curr_event->flags & EV_ERROR)
			{
				error_handling("flag error need to improve");
			}
			else if (curr_event->filter == EVFILT_READ)
			{
				if (m_fd_pool[curr_event->ident]->getFdType() == FD_SERVER)
				{
					int serv_fd = m_fd_pool[curr_event->ident]->getFd();
					int clnt_sock;

					if ((clnt_sock = accept(serv_fd, NULL, NULL)) == -1)
					{
						error_handling("accept() error");
					}
					fcntl(clnt_sock, F_SETFL, O_NONBLOCK);
					struct timeval tv;
					tv.tv_sec = 60;
					tv.tv_usec = 0;
					if (setsockopt(clnt_sock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval)) < 0)
						return (error_handling("recv time out set fail"));
					if (setsockopt(clnt_sock, SOL_SOCKET, SO_SNDTIMEO, (struct timeval*)&tv, sizeof(struct timeval)) < 0)
						return (error_handling("send time out set fail"));

					change_events(m_change_list, clnt_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					change_events(m_change_list, clnt_sock, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

					Client *clnt = new Client((dynamic_cast<Server *>(m_fd_pool[serv_fd])), clnt_sock);
					m_fd_pool[clnt_sock] = clnt;

				}
				else if (m_fd_pool[curr_event->ident]->getFdType() == FD_CLIENT)
				{
					Client *clnt = dynamic_cast<Client *>(m_fd_pool[curr_event->ident]);
					clnt->setLastTime(call_time());
					char buf[BUFSIZE];
					int n = 1;
					memset(buf, 0, BUFSIZE);
					if ((n = recv(curr_event->ident, buf, BUFSIZE-1, 0)) == -1)
						error_handling("read() error");

					if (n == 0)
					{
						deleteFdPool(m_fd_pool[curr_event->ident]);
					}
					else if (n > 0)
					{
						buf[n] = '\0';
						clnt->appendOrigin(buf);
						if(clnt->getCStatus() == REQUEST_RECEIVING && clnt->parseRequest())
						{
							clnt->setCStatus(MAKE_RESPONSE);
							clnt->makeResponse();
						}
					}
				}
				else if (m_fd_pool[curr_event->ident]->getFdType() == FD_RESOURCE)
				{
					Resource *rsc = dynamic_cast<Resource *>(m_fd_pool[curr_event->ident]);
					e_rsc_status ret = rsc->isReady();
					if (ret == READY)
					{
						char buff[BUFSIZE];
						unsigned long n = 0;
						memset(buff, 0, BUFSIZE);
						if ((n = read(curr_event->ident, buff, BUFSIZE-1)) < 0)
						{
							error_handling("read() error in Resources");
						}
						buff[n] = '\0';
						rsc->getRawData() += buff;
						if (n < BUFSIZE-1)
						{
							rsc->getClient()->setCStatus(FILE_READ_DONE);
							rsc->getClient()->makeResponse();
							deleteFdPool(m_fd_pool[curr_event->ident]);
						}
					}
					else if (ret == CGI_CRASH)
					{
						rsc->getClient()->getResponse().makeErrorResponse(500);
						deleteFdPool(rsc);
					}					
				}
			}
			else if (curr_event->filter == EVFILT_WRITE)
			{
				if (m_fd_pool[curr_event->ident]->getFdType() == FD_CLIENT)
				{
					Client* clnt = dynamic_cast<Client *>(m_fd_pool[curr_event->ident]);
					// std::cout << call_time() - clnt->getLastTime() << std::endl;
					// if (call_time() - clnt->getLastTime() > m_timeout)
					// {
					// 	clnt->getResponse().makeErrorResponse(408);
					// 	clnt->getResponse().setDisconnect(true);
					// }
					if (clnt->getCStatus() == MAKE_RESPONSE_DONE)
					{
						size_t n;

						Response &rsp = clnt->getResponse();
						n = write(curr_event->ident, rsp.getMessage().c_str(), rsp.getMessage().length());
						if (n < 0)
							error_handling("client write() error");
						if (n < rsp.getMessage().length())
						{
							rsp.getMessage().erase(0, n);
						}
						else
						{
							if (rsp.getDisconnect())
								deleteFdPool(clnt);
							else
							{
								clnt->setCStatus(REQUEST_RECEIVING);

								clnt->initRequestandResponse();
							}
						}
					}
				}
				else if (m_fd_pool[curr_event->ident]->getFdType() == FD_RESOURCE)
				{
					Resource* res = dynamic_cast<Resource *>(m_fd_pool[curr_event->ident]);
					size_t n = 0;
					
					n = write(curr_event->ident, res->getRawData().c_str(), (res->getRawData().length()));
					if (n < 0)
						error_handling("resource write error");
					if (n < res->getRawData().length())
					{
						res->getRawData().erase(0,n);
					}
					else
					{
						res->getClient()->setCStatus(FILE_WRITE_DONE);
						res->getClient()->makeResponse();
						deleteFdPool(res);
					}
				}
			}
		}
		usleep(5);
	}
}


std::vector<struct kevent>& Webserv::getChangeList()
{
	return m_change_list;
}

void Webserv::addFdPool(FdBase* res)
{
	if (m_fd_pool[res->getFd()] != NULL)
	{
		std::cout << "duplicated fd" << std::endl;
	}
	m_fd_pool[res->getFd()] = res;
}

// fd close, delete resourceList, m_fd_pool[instance] = NULL, delete instance
void Webserv::deleteFdPool(FdBase* instance)
{
	if (instance == NULL)
		return ;
		
	close(instance->getFd());
	if (instance->getFdType() == FD_CLIENT)
	{
		Client *clnt = dynamic_cast<Client *>(instance);
		std::list<Resource *> &rspList = clnt->getResponse().getResourceList();
		if (clnt->getResponse().getResourceList().size() > 1)
		{
			for (std::list<Resource *>::iterator it = rspList.begin(); it != rspList.end(); it++)
			{
				if (*it)
					deleteFdPool(dynamic_cast<FdBase*>(*it));
			}
		}
		// std::cout << "Client >> close fd : " <<instance->getFd() << std::endl;
	}
	else if (instance->getFdType() == FD_RESOURCE)
	{
		Resource * res = dynamic_cast<Resource *>(instance);
		Client *clnt = res->getClient();
		if (clnt)
		{
			std::list<Resource *> &rspList = clnt->getResponse().getResourceList();
			std::list<Resource *>::iterator it = std::find(rspList.begin(), rspList.end(), res);
			if (it != rspList.end())
			{
				rspList.erase(it);
			}
		}
		// std::cout << "Resource >> close fd : " <<instance->getFd() << std::endl;
	}
	// if (instance->getFdType() == FD_SERVER)
		// std::cout << "Server >> close fd : " << instance->getFd() << std::endl;
	m_fd_pool[instance->getFd()] = NULL;
	if (instance->getFdType() != FD_SERVER)
		delete instance;
}

void Webserv::signalExit()
{
	for (std::vector<FdBase*>::iterator it = m_fd_pool.begin(); it != m_fd_pool.end(); it++)
	{
		if (*it != NULL)
			deleteFdPool(*it);
	}
}

unsigned long Webserv::call_time(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}