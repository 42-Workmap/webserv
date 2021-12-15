#include "../incs/Config.hpp"

int main(int argc, char **argv)
{
	Config config;
	if (argc < 2)
		config.parsingConfig(std::string("./configs/default.config"));
	if (argc >= 2)
		config.parsingConfig(std::string(argv[1]));
}