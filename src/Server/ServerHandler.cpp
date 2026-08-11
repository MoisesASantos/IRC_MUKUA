/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 15:18:24 by sgaspar           #+#    #+#             */
/*   Updated: 2026/08/07 12:55:39 by emjoao           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    else if (!client->IsRegister()) client->sendData("451 :You have not registered");
    else if (cmd == "JOIN") cmdJoin(client, args);
    else if (cmd == "PRIVMSG") cmdPrivmsg(client, args);
    else if (cmd == "KICK") cmdKick(client, args);
    else if (cmd == "INVITE") cmdInvite(client, args);
    else if (cmd == "TOPIC") cmdTopic(client, args);
    else if (cmd == "MODE") cmdMode(client, args);
    else if (cmd == "PING")
    {
        std::string token;

        if (args.size() > 1)
            token = args[1];

        client->sendData(":ircserv PONG :" + token);
    }else client->sendData(":ircserv 421 " + cmd + " :Unknown command");
}

void	ServerHandler::cmdPass(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 2) { client->sendData("461 PASS :Not enough parameters"); return; }
    if (args[1] == server->GetPassword()) client->SetAuth(true);
    else client->sendData(":ircserv 464 * :Password incorrect");
}

void tryRegister(Client* client)
{
    if (client->IsAuth() &&
        !client->GetNickname().empty() &&
        !client->GetUsername().empty() &&
        !client->IsRegister())
    {
        client->SetRegister(true);

        client->sendData(
            ":ircserv 001 " +
            client->GetNickname() +
            " :Welcome to the Internet Relay Network"
        );
    }
}

void	ServerHandler::cmdNick(Client* client, const std::vector<std::string>& args) {
    if (!client->IsAuth()) return;
    if (args.size() < 2) { client->sendData(":ircserv 431 :No nickname given"); return; }
    if (server->GetClientByNick(args[1])) { client->sendData(":ircserv 433 * " + args[1] + " :Nickname is already in use"); return; }
    client->SetNickname(args[1]);
    tryRegister(client);
}

void	ServerHandler::cmdUser(Client* client, const std::vector<std::string>& args) {
    if (!client->IsAuth()) return;
    if (args.size() < 5) { client->sendData(":ircserv 461 USER :Not enough parameters"); return; }
    client->SetUsername(args[1]);
    tryRegister(client);
}

void	ServerHandler::cmdJoin(Client* client, const std::vector<std::string>& args) {
	if (args.size() < 2) return;
	std::string name = args[1];
	if (channels->find(name) == channels->end()) {
		Channel novo = Channel(name);
		novo.addOperator(client);
		channels->insert(std::make_pair(name, novo));
	}
	Channel* ch = server->GetChannel(name);
    if (ch->isInviteOnly() && !ch->isInvited(client)) {
        client->sendData(":ircserv 473 " + client->GetNickname() + " " + name + " :Cannot join channel (+i)");
        return;
    }
    if (!ch->getPassword().empty() && (args.size() < 3 || args[2] != ch->getPassword())) {
        client->sendData(":ircserv 475 " + client->GetNickname() + " " + name + " :Cannot join channel (+k)");
        return;
    }
    if (ch->getUserLimit() > 0 && ch->getMemberCount() >= ch->getUserLimit()) {
        client->sendData(":ircserv 471 " + client->GetNickname() + " " + name + " :Cannot join channel (+l)");
        return;
    }
    ch->addMember(client);
    std::string joinMsg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost JOIN " + name;
    ch->broadcast(joinMsg);
}

void ServerHandler::cmdPrivmsg(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return;
    std::string target = args[1];
    std::string msg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost PRIVMSG " + target + " :" + args[2];

    if (target[0] == '#') {
        if (channels->find(target) != channels->end() && (*channels)[target].hasMember(client))
            (*channels)[target].broadcast(msg, client);
    } else {
        Client* dest = server->GetClientByNick(target);
        if (dest) dest->sendData(msg);
        else client->sendData(":ircserv 401 " + client->GetNickname() + " " + target + " :No such nick/channel");
    }
}

void ServerHandler::cmdKick(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return;
    std::string chName = args[1];
    std::string targetNick = args[2];
    if (channels->find(chName) == channels->end()) return;
    Channel* ch = &(*channels)[chName];
    if (!ch->isOperator(client)) { client->sendData(":ircserv 482 "+client->GetNickname()+" "+chName+" :You're not channel operator"); return; }

    Client* dest = server->GetClientByNick(targetNick);
    if (dest && ch->hasMember(dest)) {
        std::string kickMsg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost KICK " + chName + " " + targetNick + " :" + (args.size() > 3 ? args[3] : "Kicked");
        ch->broadcast(kickMsg);
        ch->removeMember(dest);
    }
}

void ServerHandler::cmdInvite(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return;
    std::string targetNick = args[1];
    std::string chName = args[2];
    if (channels->find(chName) == channels->end()) return;
    Channel* ch = &(*channels)[chName];
    if (!ch->isOperator(client)) { client->sendData(":ircserv 482 "+client->GetNickname()+" "+chName+" :You're not channel operator"); return; }
    Client* dest = server->GetClientByNick(targetNick);
    if (dest) {
        ch->invite(dest);
        client->sendData(":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost INVITE " + targetNick + " :" + chName);
    }
}

void ServerHandler::cmdTopic(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 2) return;
    std::string chName = args[1];
    if (channels->find(chName) == channels->end()) return;
    Channel* ch = &(*channels)[chName];

    if (args.size() == 2) {
        client->sendData(":ircserv 332 " + client->GetNickname() + " " + chName + " :" + ch->getTopic());
    } else {
        if (ch->getTopic().empty()) {
            client->sendData(":ircserv 331 " + client->GetNickname() + " " + chName + " :No topic is set");
            return;
        }
        if (ch->isTopicRestricted() && !ch->isOperator(client)) {
            client->sendData(":ircserv 482 " + client->GetNickname() + " " + chName + " :You're not channel operator");
            return;
        }
        ch->setTopic(args[2]);
        ch->broadcast(":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost TOPIC " + chName + " :" + args[2]);
    }
}

void ServerHandler::cmdMode(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return;
    std::string chName = args[1];
    std::string mode = args[2];
    if (channels->find(chName) == channels->end()) return;
    Channel* ch = &(*channels)[chName];
    if (!ch->isOperator(client)) { client->sendData(":ircserv 482 " +client->GetNickname() + " " +chName + " :You're not channel operator"); return; }

    bool add = (mode[0] == '+');
    char m = mode[1];

    if (m == 'i') ch->setInviteOnly(add);
    else if (m == 't') ch->setTopicRestricted(add);
    else if (m == 'k' && args.size() > 3) ch->setPassword(add ? args[3] : "");
    else if (m == 'l') ch->setUserLimit(add && args.size() > 3 ? std::atoi(args[3].c_str()) : 0);
    else if (m == 'o' && args.size() > 3) {
        Client* dest = server->GetClientByNick(args[3]);
        if (dest && ch->hasMember(dest)) {
            if (add) ch->addOperator(dest);
            else ch->removeOperator(dest);
        }
    }
    std::string msg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost MODE " + chName + " " + mode + (args.size() > 3 ? " " + args[3] : "");
    ch->broadcast(msg);
}