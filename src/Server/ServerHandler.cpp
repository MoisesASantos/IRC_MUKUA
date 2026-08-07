/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 15:18:24 by sgaspar           #+#    #+#             */
/*   Updated: 2026/08/07 19:04:41 by sgaspar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./ServerHandler.hpp"
#include <iomanip>  // Para setfill e setw
#include <sstream>  // Para stringstream
#include <cstdlib>  // Para atoi

ServerHandler::ServerHandler() {}
ServerHandler::ServerHandler(const ServerHandler& other) { (void)other; }
ServerHandler& ServerHandler::operator=(const ServerHandler & other) {
    (void)other;
    return *this;
}
ServerHandler::~ServerHandler() {}

void ServerHandler::setServer(Server *server) {
    if (server != NULL){
        this->server = server;
        channels = server->GetAllChannel();
    }
}

// NOTA: Os valores padrão (=" ") devem ficar no ServerHandler.hpp
void ServerHandler::sendReply(Client* client, NumericReplies code, const std::string& arg1, const std::string& arg2) {
    std::stringstream ss;
    std::string nick = client->GetNickname().empty() ? "*" : client->GetNickname();

    ss << ":ircserv " << std::setfill('0') << std::setw(3) << code << " " << nick << " ";

    switch (code) {
        // --- INFORMATIVOS ---
        case RPL_WELCOME:           ss << ":Welcome to the Internet Relay Network " << nick; break;
        case RPL_YOURHOST:          ss << ":Your host is ircserv, running version 1.0"; break;
        case RPL_CREATED:           ss << ":This server was created today"; break;
        case RPL_MYINFO:            ss << "ircserv 1.0 io itkol"; break;
        case RPL_CHANNELMODEIS:     ss << arg1 << " " << arg2; break;
        case RPL_NOTOPIC:           ss << arg1 << " :No topic is set"; break;
        case RPL_TOPIC:             ss << arg1 << " :" << arg2; break;
        case RPL_INVITING:          ss << arg1 << " " << arg2; break;
        case RPL_NAMREPLY:          ss << "= " << arg1 << " :" << arg2; break;
        case RPL_ENDOFNAMES:        ss << arg1 << " :End of /NAMES list"; break;

        // --- ERROS ---
        case ERR_NOSUCHNICK:        ss << arg1 << " :No such nick/channel"; break;
        case ERR_NOSUCHCHANNEL:     ss << arg1 << " :No such channel"; break;
        case ERR_UNKNOWNCOMMAND:    ss << arg1 << " :Unknown command"; break;
        case ERR_NONICKNAMEGIVEN:   ss << ":No nickname given"; break;
        case ERR_NICKNAMEINUSE:     ss << arg1 << " :Nickname is already in use"; break;
        case ERR_USERNOTINCHANNEL:  ss << arg1 << " " << arg2 << " :They aren't on that channel"; break;
        case ERR_NOTONCHANNEL:      ss << arg1 << " :You're not on that channel"; break;
        case ERR_USERONCHANNEL:     ss << arg1 << " " << arg2 << " :is already on channel"; break;
        case ERR_NOTREGISTERED:     ss << ":You have not registered"; break;
        case ERR_NEEDMOREPARAMS:    ss << arg1 << " :Not enough parameters"; break;
        case ERR_PASSWDMISMATCH:    ss << ":Password incorrect"; break;
        case ERR_CHANNELISFULL:     ss << arg1 << " :Cannot join channel (+l)"; break;
        case ERR_INVITEONLYCHAN:    ss << arg1 << " :Cannot join channel (+i)"; break;
        case ERR_BADCHANNELKEY:     ss << arg1 << " :Cannot join channel (+k)"; break;
        case ERR_CHANOPRIVSNEEDED:  ss << arg1 << " :You're not channel operator"; break;
        default:                    ss << arg1 << " :Unknown numeric reply"; break;
    }

    client->sendData(ss.str());
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
    else if (!client->IsRegister()) sendReply(client, ERR_NOTREGISTERED);
    else if (cmd == "JOIN") cmdJoin(client, args);
    else if (cmd == "PRIVMSG") cmdPrivmsg(client, args);
    else if (cmd == "KICK") cmdKick(client, args);
    else if (cmd == "INVITE") cmdInvite(client, args);
    else if (cmd == "TOPIC") cmdTopic(client, args);
    else if (cmd == "MODE") cmdMode(client, args);
    else if (cmd == "PING") {
        std::string token = (args.size() > 1) ? args[1] : "";
        client->sendData(":ircserv PONG :" + token); // Ping não é numérico, usa sendData
    }
    else sendReply(client, ERR_UNKNOWNCOMMAND, cmd);
}

void ServerHandler::cmdPass(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 2) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);
    if (args[1] == server->GetPassword()) client->SetAuth(true);
    else sendReply(client, ERR_PASSWDMISMATCH);
}

void ServerHandler::tryRegister(Client* client) {
    if (client->IsAuth() && !client->GetNickname().empty() && 
        !client->GetUsername().empty() && !client->IsRegister()) {

        client->SetRegister(true);
        // Respostas oficiais de boas vindas
        sendReply(client, RPL_WELCOME);
        sendReply(client, RPL_YOURHOST);
        sendReply(client, RPL_CREATED);
        sendReply(client, RPL_MYINFO);
    }
}

void ServerHandler::cmdNick(Client* client, const std::vector<std::string>& args) {
    if (!client->IsAuth()) return;
    if (args.size() < 2) return sendReply(client, ERR_NONICKNAMEGIVEN);
    if (server->GetClientByNick(args[1])) return sendReply(client, ERR_NICKNAMEINUSE, args[1]);

    client->SetNickname(args[1]);
    tryRegister(client);
}

void ServerHandler::cmdUser(Client* client, const std::vector<std::string>& args) {
    if (!client->IsAuth()) return;
    if (args.size() < 5) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);

    client->SetUsername(args[1]);
    tryRegister(client);
}

void ServerHandler::cmdJoin(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 2) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);
    std::string name = args[1];

    if (channels->find(name) == channels->end()) {
        Channel novo = Channel(name);
        novo.addOperator(client);
        channels->insert(std::make_pair(name, novo));
    }

    Channel* ch = server->GetChannel(name);
    if (ch->isInviteOnly() && !ch->isInvited(client)) return sendReply(client, ERR_INVITEONLYCHAN, name);
    if (!ch->getPassword().empty() && (args.size() < 3 || args[2] != ch->getPassword())) return sendReply(client, ERR_BADCHANNELKEY, name);
    if (ch->getUserLimit() > 0 && ch->getMemberCount() >= ch->getUserLimit()) return sendReply(client, ERR_CHANNELISFULL, name);

    ch->addMember(client);
    std::string joinMsg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost JOIN " + name;
    ch->broadcast(joinMsg);

    // Atualiza a janela do Irssi com a lista de usuários
    sendReply(client, RPL_NAMREPLY, name, ch->getNamesList());
    sendReply(client, RPL_ENDOFNAMES, name);
}

void ServerHandler::cmdPrivmsg(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);
    std::string target = args[1];
    std::string msg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost PRIVMSG " + target + " :" + args[2];

    if (target[0] == '#') {
        if (channels->find(target) != channels->end() && (*channels)[target].hasMember(client))
            (*channels)[target].broadcast(msg, client);
    } else {
        Client* dest = server->GetClientByNick(target);
        if (dest) dest->sendData(msg);
        else sendReply(client, ERR_NOSUCHNICK, target);
    }
}

void ServerHandler::cmdKick(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);
    std::string chName = args[1];
    std::string targetNick = args[2];

    if (channels->find(chName) == channels->end()) return sendReply(client, ERR_NOSUCHCHANNEL, chName);
    Channel* ch = &(*channels)[chName];
    if (!ch->isOperator(client)) return sendReply(client, ERR_CHANOPRIVSNEEDED, chName);

    Client* dest = server->GetClientByNick(targetNick);
    // Verifica se o alvo existe e se está no canal
    if (!dest || !ch->hasMember(dest)) return sendReply(client, ERR_USERNOTINCHANNEL, targetNick, chName);

    std::string kickMsg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost KICK " + chName + " " + targetNick + " :" + (args.size() > 3 ? args[3] : "Kicked");
    ch->broadcast(kickMsg);
    ch->removeMember(dest);
}

void ServerHandler::cmdInvite(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);
    std::string targetNick = args[1];
    std::string chName = args[2];

    if (channels->find(chName) == channels->end()) return sendReply(client, ERR_NOSUCHCHANNEL, chName);
    Channel* ch = &(*channels)[chName];
    if (!ch->isOperator(client)) return sendReply(client, ERR_CHANOPRIVSNEEDED, chName);

    Client* dest = server->GetClientByNick(targetNick);
    if (!dest) return sendReply(client, ERR_NOSUCHNICK, targetNick);
    if (ch->hasMember(dest)) return sendReply(client, ERR_USERONCHANNEL, targetNick, chName);

    ch->invite(dest);
    // Confirmação para quem enviou o convite
    sendReply(client, RPL_INVITING, targetNick, chName);
    // Envio do convite real para o destinatário
    dest->sendData(":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost INVITE " + targetNick + " :" + chName);
}

void ServerHandler::cmdTopic(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 2) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);
    std::string chName = args[1];

    if (channels->find(chName) == channels->end()) return sendReply(client, ERR_NOSUCHCHANNEL, chName);
    Channel* ch = &(*channels)[chName];

    if (args.size() == 2) {
        if (ch->getTopic().empty()) sendReply(client, RPL_NOTOPIC, chName);
        else sendReply(client, RPL_TOPIC, chName, ch->getTopic());
    } else {
        if (ch->isTopicRestricted() && !ch->isOperator(client)) return sendReply(client, ERR_CHANOPRIVSNEEDED, chName);

        ch->setTopic(args[2]);
        ch->broadcast(":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost TOPIC " + chName + " :" + args[2]);
    }
}

void ServerHandler::cmdMode(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 3) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);
    std::string chName = args[1];
    std::string mode = args[2];

    if (channels->find(chName) == channels->end()) return sendReply(client, ERR_NOSUCHCHANNEL, chName);
    Channel* ch = &(*channels)[chName];
    if (!ch->isOperator(client)) return sendReply(client, ERR_CHANOPRIVSNEEDED, chName);

    if (mode.length() < 2 || (mode[0] != '+' && mode[0] != '-'))
        return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);

    bool add = (mode[0] == '+');
    char m = mode[1];
    std::string paramExtra = "";

    if (m == 'i') ch->setInviteOnly(add);
    else if (m == 't') ch->setTopicRestricted(add);
    else if (m == 'k') {
        if (args.size() < 4) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);
        if (add) { 
            ch->setPassword(args[3]);
            paramExtra = " " + args[3];
        } else {   
            if (args[3] == ch->getPassword()) {
                ch->setPassword(""); 
                paramExtra = " *";
            }
        }
    }
    else if (m == 'l') {
        if (add) { 
            if (args.size() < 4) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);
            ch->setUserLimit(std::atoi(args[3].c_str()));
            paramExtra = " " + args[3];
        } else ch->setUserLimit(0); 
    }
    else if (m == 'o') {
        if (args.size() < 4) return sendReply(client, ERR_NEEDMOREPARAMS, args[0]);

        Client* dest = server->GetClientByNick(args[3]);
        if (!dest) return sendReply(client, ERR_NOSUCHNICK, args[3]);
        if (!ch->hasMember(dest)) return sendReply(client, ERR_USERNOTINCHANNEL, args[3], chName);

        if (add) ch->addOperator(dest);
        else ch->removeOperator(dest);
    }
    else return; // Modo desconhecido, ignora silenciosamente (ou implemente ERR_UNKNOWNMODE)

    std::string msg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@localhost MODE " + chName + " " + mode + paramExtra;
    ch->broadcast(msg);
}
