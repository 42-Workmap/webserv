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
					char buf[1024];
					int n = 0;
					Client *clnt = dynamic_cast<Client *>(m_fd_pool[curr_event->ident]);
					memset(buf, 0, 1024);
					if ((n = read(curr_event->ident, buf, sizeof(buf))) == -1)
						error_handling("read() error");
					else if (n == 0)
					{
						close(curr_event->ident);
						delete m_fd_pool[curr_event->ident];
						std::cout << std::endl << "delete client" << std::endl;
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