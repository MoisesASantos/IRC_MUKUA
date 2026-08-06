#include "./Bot.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>

// Forma Canônica
Bot::Bot() : _fd(-1), _ip(""), _port(0), _password("") {}

Bot::Bot(const Bot& other) { *this = other; }

Bot& Bot::operator=(const Bot& other) {
	if (this != &other) {
		_fd = other._fd;
		_ip = other._ip;
		_port = other._port;
		_password = other._password;
	}
	return *this;
}

// Construtor Lógico
Bot::Bot(const std::string& ip, int port, const std::string& password)
: _ip(ip), _port(port), _password(password) {
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = inet_addr(_ip.c_str());

	if (connect(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		throw std::runtime_error("Bot connection failed");
	}
}

Bot::~Bot() { 
	if (_fd != -1) close(_fd); 
}

void Bot::run() {
	std::string login = "PASS " + _password + "\r\nNICK HelperBot\r\nUSER bot 0 * :I am a Bot\r\n";
	send(_fd, login.c_str(), login.length(), 0);

	char buf[512];
	while (true) {
		ssize_t bytes = recv(_fd, buf, sizeof(buf) - 1, 0);
		if (bytes <= 0) break;
		buf[bytes] = '\0';
		std::string msg(buf);

		size_t priv = msg.find("PRIVMSG");
		if (priv != std::string::npos && msg.find("!ping") != std::string::npos) {
			size_t senderEnd = msg.find('!');
			if (senderEnd != std::string::npos) {
				std::string sender = msg.substr(1, senderEnd - 1);
				std::string reply = "PRIVMSG " + sender + " :PONG! I am alive!\r\n";
				send(_fd, reply.c_str(), reply.length(), 0);
			}
		}
	}
}
