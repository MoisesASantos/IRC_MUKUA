#pragma once

#include <string>
#include <vector>
#include "../Client/Client.hpp"
#include "./Server.hpp"

class	ServerHandler {
	private:
	Server	*server;
	std::map<std::string, Channel> *channels;

	public:
	ServerHandler();
	ServerHandler(const ServerHandler& other);
	ServerHandler& operator=(const ServerHandler & other);
	~ServerHandler();

	void	setServer(Server *server);

	void processCommand(Client* client, const std::string& msg);
	std::vector<std::string> splitMessage(const std::string& msg);

	void cmdPass(Client* client, const std::vector<std::string>& args);
	void cmdNick(Client* client, const std::vector<std::string>& args);
	void cmdUser(Client* client, const std::vector<std::string>& args);
	void cmdJoin(Client* client, const std::vector<std::string>& args);
	void cmdPrivmsg(Client* client, const std::vector<std::string>& args);
	void cmdKick(Client* client, const std::vector<std::string>& args);
	void cmdInvite(Client* client, const std::vector<std::string>& args);
	void cmdTopic(Client* client, const std::vector<std::string>& args);
	void cmdMode(Client* client, const std::vector<std::string>& args);
};
