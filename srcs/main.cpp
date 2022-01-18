#include "../incs/Config.hpp"
#include <signal.h>

void signalhandler(int sigint)
{
	if (sigint == 2)
	{
		std::cout << "signal 보내 signal  보내!!" << std::endl;
		Config::getConfig()->getWebserv()->signalExit();
	}
	exit(1);  // 숫자 뭐해야함?
}

int main(int argc, char **argv)
{
	Config *config;

	config = Config::getConfig();
	if (argc < 2)
		config->parsingConfig(std::string("./configs/default.config"));
	if (argc >= 2)
		config->parsingConfig(std::string(argv[1]));
	Webserv webserv;

	signal(SIGINT, signalhandler);
	config->setWebserv(&webserv);
	config->getWebserv()->startServer();
}