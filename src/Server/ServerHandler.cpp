#include "./ServerHandler.hpp"

ServerHandler::ServerHandler() {}
ServerHandler::ServerHandler(const ServerHandler& other) { (void)other; }
ServerHandler& ServerHandler::operator=(const ServerHandler & other) {
	(void)other;
	return *this;
}
ServerHandler::~ServerHandler() {}

void	ServerHandler::setServer(Server *server) {
	if (server != NULL){
		this->server = server;
		channels = server->GetAllChannel();
	}
}

std::vector<std::string> ServerHandler::splitMessage(const std::string& msg) {
	std::vector<std::string> args;
	size_t i = 0;
	while (i < msg.length()) {
		while (i < msg.length() && msg[i] == ' ') i++;
		if (i >= msg.length()) break;
		if (msg[i] == ':') {
			args.push_back(msg.substr(i + 1));
			break;
		}
		size_t end = msg.find(' ', i);
		if (end == std::string::npos) {
			args.push_back(msg.substr(i));
			break;
		}
		args.push_back(msg.substr(i, end - i));
		i = end;
	}
	return args;
}

void ServerHandler::processCommand(Client* client, const std::string& msg) {
    std::vector<std::string> args = splitMessage(msg);
    if (args.empty()) return;

    std::string cmd = args[0];
    for (size_t i = 0; i < cmd.length(); ++i) cmd[i] = toupper(cmd[i]);

    if (cmd == "PASS") cmdPass(client, args);
    else if (cmd == "NICK") cmdNick(client, args);
    else if (cmd == "USER") cmdUser(client, args);
    else if (!client->IsRegister()) client->sendData("451 :You have not registered \r\n");
    else if (cmd == "JOIN") cmdJoin(client, args);
    else if (cmd == "PRIVMSG") cmdPrivmsg(client, args);
    else if (cmd == "KICK") cmdKick(client, args);
    else if (cmd == "INVITE") cmdInvite(client, args);
    else if (cmd == "TOPIC") cmdTopic(client, args);
    else if (cmd == "MODE") cmdMode(client, args);
    else if (cmd == "PING") client->sendData("PONG :" + (args.size() > 1 ? args[1] : "") + "\r\n");
}

void	ServerHandler::cmdPass(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 2) { client->sendData("461 PASS :Not enough parameters \r\n"); return; }
    if (args[1] == server->GetPassword()) client->SetAuth(true);
}

void	ServerHandler::cmdNick(Client* client, const std::vector<std::string>& args) {
    if (!client->IsAuth()) return;
    if (args.size() < 2) { client->sendData("431 :No nickname given \r\n"); return; }
    if (server->GetClientByNick(args[1])) { client->sendData("433 " + args[1] + " :Nickname is already in use \r\n"); return; }
    client->SetNickname(args[1]);
    if (!client->GetUsername().empty() && !client->IsRegister()) {
        client->SetRegister(true);
        client->sendData("001 " + client->GetNickname() + " :Welcome to ft_irc \r\n");
    }
}

void	ServerHandler::cmdUser(Client* client, const std::vector<std::string>& args) {
    if (!client->IsAuth()) return;
    if (args.size() < 5) { client->sendData("461 USER :Not enough parameters \r\n"); return; }
    client->SetUsername(args[1]);
    if (!client->GetNickname().empty() && !client->IsRegister()) {
        client->SetRegister(true);
        client->sendData("001 " + client->GetNickname() + " :Welcome to ft_irc \r\n");
    }
}

void	ServerHandler::cmdJoin(Client* client, const std::vector<std::string>& args) {
	if (args.size() < 2) return;
	std::string name = args[1];
	if (channels->find(name) == channels->end()) {
		Channel novo = Channel(name);
		novo.addOperator(client);
		channels->insert(std::make_pair(name, Channel(name)));
		// _channels[name] = new Channel(name);
		// _channels[name]->addOperator(client);
	}
	Channel* ch = server->GetChannel(name);
    if (ch->isInviteOnly() && !ch->isInvited(client)) {
        client->sendData("473 " + name + " :Cannot join channel (+i) \r\n");
        return;
    }
    if (!ch->getPassword().empty() && (args.size() < 3 || args[2] != ch->getPassword())) {
        client->sendData("475 " + name + " :Cannot join channel (+k) \r\n");
        return;
    }
    if (ch->getUserLimit() > 0 && ch->getMemberCount() >= ch->getUserLimit()) {
        client->sendData("471 " + name + " :Cannot join channel (+l) \r\n");
        return;
    }
    ch->addMember(client);
    std::string joinMsg = ":" + client->GetNickname() + " JOIN " + name;
    ch->broadcast(joinMsg);
    client->sendData(joinMsg + "\r\n");
}

void ServerHandler::cmdPrivmsg(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return;
    std::string target = args[1];
    std::string msg = ":" + client->GetNickname() + " PRIVMSG " + target + " :" + args[2] + " \r\n";

    if (target[0] == '#') {
        if (channels->find(target) != channels->end() && (*channels)[target].hasMember(client))
            (*channels)[target].broadcast(msg, client);
    } else {
        Client* dest = server->GetClientByNick(target);
        if (dest) dest->sendData(msg);
        else client->sendData("401 " + target + " :No such nick/channel \r\n");
    }
}

void ServerHandler::cmdKick(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return;
    std::string chName = args[1];
    std::string targetNick = args[2];
    if (channels->find(chName) == channels->end()) return;
    Channel ch = (*channels)[chName];
    if (!ch.isOperator(client)) { client->sendData("482 :You're not channel operator \r\n"); return; }
    
    Client* dest = server->GetClientByNick(targetNick);
    if (dest && ch.hasMember(dest)) {
        std::string kickMsg = ":" + client->GetNickname() + " KICK " + chName + " " + targetNick + " :" + (args.size() > 3 ? args[3] : "Kicked \r\n");
        ch.broadcast(kickMsg);
        dest->sendData(kickMsg);
        ch.removeMember(dest);
    }
}

void ServerHandler::cmdInvite(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return;
    std::string targetNick = args[1];
    std::string chName = args[2];
    if (channels->find(chName) == channels->end()) return;
    Channel ch = (*channels)[chName];
    if (!ch.isOperator(client)) { client->sendData("482 :You're not channel operator \r\n"); return; }
    Client* dest = server->GetClientByNick(targetNick);
    if (dest) {
        ch.invite(dest);
        dest->sendData(":" + client->GetNickname() + " INVITE " + targetNick + " " + chName + "\r\n");
    }
}

void ServerHandler::cmdTopic(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 2) return;
    std::string chName = args[1];
    if (channels->find(chName) == channels->end()) return;
    Channel ch = (*channels)[chName];
    
    if (args.size() == 2) {
        client->sendData("332 " + client->GetNickname() + " " + chName + " :" + ch.getTopic() + "\r\n");
    } else {
        if (ch.isTopicRestricted() && !ch.isOperator(client)) {
            client->sendData("482 :You're not channel operator \r\n");
            return;
        }
        ch.setTopic(args[2]);
        ch.broadcast(":" + client->GetNickname() + " TOPIC " + chName + " :" + args[2] + "\r\n");
        client->sendData(":" + client->GetNickname() + " TOPIC " + chName + " :" + args[2] + "\r\n");
    }
}

void ServerHandler::cmdMode(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return;
    std::string chName = args[1];
    std::string mode = args[2];
    if (channels->find(chName) == channels->end()) return;
    Channel ch = (*channels)[chName];
    if (!ch.isOperator(client)) { client->sendData("482 :You're not channel operator \r\n"); return; }

    bool add = (mode[0] == '+');
    char m = mode[1];
    
    if (m == 'i') ch.setInviteOnly(add);
    else if (m == 't') ch.setTopicRestricted(add);
    else if (m == 'k' && args.size() > 3) ch.setPassword(add ? args[3] : "");
    else if (m == 'l') ch.setUserLimit(add && args.size() > 3 ? std::atoi(args[3].c_str()) : 0);
    else if (m == 'o' && args.size() > 3) {
        Client* dest = server->GetClientByNick(args[3]);
        if (dest && ch.hasMember(dest)) {
            if (add) ch.addOperator(dest);
            else ch.removeOperator(dest);
        }
    }
    std::string msg = ":" + client->GetNickname() + " MODE " + chName + " " + mode + (args.size() > 3 ? " " + args[3] : "") + "\r\n";
    ch.broadcast(msg);
    client->sendData(msg);
}
