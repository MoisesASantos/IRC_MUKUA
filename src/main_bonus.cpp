#include "./Bot/Bot.hpp"
#include <iostream>
#include <cstdlib>

int	main(int argc, char **argv) {
	if (argc != 4) {
		std::cerr << "Usage: ./ircbot <ip> <port> <password>" << std::endl;
		return 1;
	}
	try {
		Bot bot(argv[1], std::atoi(argv[2]), argv[3]);
		bot.run();
	} catch (std::exception &e) {
		std::cerr << "Bot Error: " << e.what() << std::endl;
	}
	return 0;
}
