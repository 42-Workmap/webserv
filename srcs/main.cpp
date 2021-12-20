#include "../incs/Config.hpp"

int main(int argc, char **argv)
{
	Config *config;

	config = Config::getConfig();
	if (argc < 2)
		config->parsingConfig(std::string("./configs/default.config"));
	if (argc >= 2)
		config->parsingConfig(std::string(argv[1]));
	Webserv webserv;

	config->setWebserv(&webserv);
	config->getWebserv()->startServer();
}