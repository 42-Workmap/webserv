#include "../incs/Webserv.hpp"

Webserv::Webserv()
{
	this->m_fd_pool.resize(MAX_FD_SIZE, NULL);
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

					change_events(m_change_list, clnt_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					change_events(m_change_list, clnt_sock, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

					Client *clnt = new Client((dynamic_cast<Server *>(m_fd_pool[serv_fd])), clnt_sock);
					std::cout << "Accepted " << clnt_sock << std::endl;
					m_fd_pool[clnt_sock] = clnt;

				}
				else if (m_fd_pool[curr_event->ident]->getFdType() == FD_CLIENT)
				{
					Client *clnt = dynamic_cast<Client *>(m_fd_pool[curr_event->ident]);
					if (clnt->getCStatus() == REQUEST_RECEIVING)
					{
						char buf[1025];
						int n = 1;
						memset(buf, 0, 1025);
						if ((n = recv(curr_event->ident, buf, sizeof(buf), 0)) == -1)
							error_handling("read() error");

						if (n == 0)
						{
							std::cout << "client read zero!!" << std::endl;
							deleteFdPool(m_fd_pool[curr_event->ident]);
						}
						else if (n > 0)
						{
							buf[n] = '\0';
							std::cout << buf << "\n";
							clnt->appendOrigin(buf);
							if(clnt->getCStatus() == REQUEST_RECEIVING && clnt->parseRequest())
							{
								std::cout << "parseRequest end" << std::endl;
								clnt->setCStatus(MAKE_RESPONSE);
								clnt->makeResponse();
							}
						}
					}
				}
				else if (m_fd_pool[curr_event->ident]->getFdType() == FD_RESOURCE)
				{
					Resource *rsc = dynamic_cast<Resource *>(m_fd_pool[curr_event->ident]);
					if (rsc->getClient()->getCStatus() == MAKE_RESPONSE)
					{
						char buff[10];
						unsigned long n = 0;
						memset(buff, 0, 10);
						if ((n = read(curr_event->ident, buff, sizeof(buff))) < 0)
						{
							error_handling("read() error in Resources");
						}
						buff[n] = '\0';
						rsc->getRawData() += buff;
						if (n < sizeof(buff))
						{
							std::cout << rsc->getRawData() << std::endl;
							rsc->getClient()->setCStatus(MAKE_RESPONSE_DONE);
							deleteFdPool(m_fd_pool[curr_event->ident]);

						}
					}
				}
			}
			else if (curr_event->filter == EVFILT_WRITE)
			{
				if (m_fd_pool[curr_event->ident]->getFdType() == FD_CLIENT)
				{
					Client* clnt = dynamic_cast<Client *>(m_fd_pool[curr_event->ident]);
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
					else if (clnt->getCStatus() == FILE_WRITE_DONE)
					{
						clnt->setCStatus(MAKE_RESPONSE_DONE);
					}
				}
				else if (m_fd_pool[curr_event->ident]->getFdType() == FD_RESOURCE)
				{
					Resource* res = dynamic_cast<Resource *>(m_fd_pool[curr_event->ident]);
					if (res->getClient()->getCStatus() == FILE_WRITING)
					{
						size_t n = 0;
						size_t w_idx = res->getWriteIndex();
						const char *curr_str = (res->getRawData().c_str() + w_idx);
						n = write(curr_event->ident, curr_str, (res->getRawData().size() - w_idx));
						if (n < 0)
							error_handling("resource write error");
						if (n < res->getRawData().size()-w_idx)
						{
							res->setWriteIndex(w_idx + n);
							// res->getRawData().erase(0,n);
						}
						else
						{
							res->getClient()->setCStatus(FILE_WRITE_DONE);
							deleteFdPool(res);
						}
					}
				}
			}
		}
	}
}


std::vector<struct kevent>& Webserv::getChangeList()
{
	return m_change_list;
}

void Webserv::addFdPool(FdBase* res)
{
	m_fd_pool[res->getFd()] = res;
}

// fd close, delete resourceList, m_fd_pool[instance] = NULL, delete instance
void Webserv::deleteFdPool(FdBase* instance)
{
	if (instance == NULL)
		return ;
	//cgi는 좀 나중에,,
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
		std::cout << instance->getFd() << " : client closed" << std::endl;
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
		std::cout << instance->getFd() << " : resource closed" << std::endl;
	}
	m_fd_pool[instance->getFd()] = NULL;
	if (instance->getFdType() != FD_SERVER)
		delete instance;
}