/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_handle.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emjoao <emjoao@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 15:02:47 by emjoao            #+#    #+#             */
/*   Updated: 2026/08/04 10:23:17 by emjoao           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/header.hpp"
#include "../Channel/Channel.hpp"

std::string ERR_UNKNOWNCOMMAND(const std::string& command)
{
    return (":server 421 " + command + " :Unknown command");
}
std::string ERR_NONICKNAMEGIVEN()
{
    return (":server 431 :No nickname given");
}
std::string ERR_NEEDMOREPARAMS(const std::string& command)
{
    return (":server 461 " + command + " :Not enough parameters");
}
std::string ERR_NOSUCHCHANNEL(const std::string& channel)
{
    return (":server 403 " + channel + " :No such channel");
}
std::string ERR_CHANOPRIVSNEEDED(const std::string& channel)
{
    return (":server 482 " + channel + " :You're not channel operator");
}
std::string ERR_NOTONCHANNEL(const std::string& channel)
{
    return (":server 442 " + channel + " :You're not on that channel");
}

std::string ERR_ALREADYREGISTRED(const std::string& nickname)
{
    return (":server 462 " + nickname + " :You're already registered");
}

std::string ERR_PASSWDMISMATCH(const std::string& nickname)
{
    return (":server 464 " + nickname + " :Password mismatch");
}

std::string ERR_NOTREGISTERED(const std::string& nickname)
{
    return (":server 451 " + nickname + " :You're not registered");
}

std::string ERR_INVITEONLYCHAN(const std::string& nickname, const std::string& channel)
{
    return (":server 473 " + nickname + " " + channel + " :You're not on that channel");
}

std::string ERR_BADCHANNELKEY(const std::string& nickname, const std::string& channel)
{
    return (":server 475 " + nickname + " " + channel + " :Bad channel key");
}

std::string ERR_CHANNELISFULL(const std::string& nickname, const std::string& channel)
{
    return (":server 471 " + nickname + " " + channel + " :Channel is full");
}

std::string ERR_BADCHANMASK(const std::string& nickname, const std::string& channel)
{
    return (":server 476 " + nickname + " " + channel + " :Bad channel mask");
}

std::string ERR_TOOMANYCHANNELS(const std::string& nickname, const std::string& channel)
{
    return (":server 407 " + nickname + " " + channel + " :You're not on that channel");
}

std::string ERR_NEEDMOREPARAMS(const std::string& nickname, const std::string& command)
{
    return (":server 461 " + nickname + " " + command + " :Not enough parameters");
}

std::string ERR_NONICKNAMEGIVEN(const std::string& nickname)
{
    return (":server 431 " + nickname + " :No nickname given");
}

std::string ERR_ERRONEUSNICKNAME(const std::string& nickname)
{
    return (":server 432 " + nickname + " :Erroneus nickname");
}

static void tryCompleteRegistration(Client& client)
{
    if (client.IsAuth() && !client.GetNickname().empty() && !client.GetUsername().empty())
    {
        client.SetRegister(true);

        const std::string& nick = client.GetNickname();
        const std::string  mask = nick + "!" + client.GetUsername() + "@" + client.GetHostname();

        // 001 RPL_WELCOME
        client.SendReply(":server 001 " + nick + " :Welcome to the IRC Network " + mask);
        // 002 RPL_YOURHOST
        client.SendReply(":server 002 " + nick + " :Your host is server, running version 1.0");
        // 003 RPL_CREATED
        client.SendReply(":server 003 " + nick + " :This server was created today");
        // 004 RPL_MYINFO
        client.SendReply(":server 004 " + nick + " server 1.0 o itkol");
    }
}

void handlePass(Server& server, Client& client, const std::vector<std::string>& params)
{
    if(client.IsRegister())
    {
        client.SendReply(ERR_ALREADYREGISTRED(client.GetNickname()));
        return;
    }
    if(params.empty())
    {
        client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "PASS"));
        return;
    }
    (void)server;

    if(client.IsRegister())
    {
        client.SendReply(ERR_ALREADYREGISTRED(client.GetNickname()));
        return;
    }
    if (params.size() < 4)
    {
        client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "USER"));
        return;
    }
    if (!client.IsAuth())
    {
        client.SendReply(ERR_PASSWDMISMATCH(client.GetNickname()));
        return;
    }
    client.SetUsername(params[0]);
    client.SetHostname(params[1]);
    client.SetRealname(params[3]);
    tryCompleteRegistration(client);
}

void handleJoin(Server& server, Client& client, const std::vector<std::string>& params)
{
    if(params[0] != server.GetPassword())
    {
        client.SendReply(ERR_PASSWDMISMATCH(client.GetNickname()));
        return;
    }
    client.SetAuth(true);
}


void handleNick(Server& server, Client& client, const std::vector<std::string>& params)
{
    if (params.empty())
    {
        client.SendReply(ERR_NONICKNAMEGIVEN());
        return;
    }

    std::map<int, Client>& clients = server.GetClientsMap();
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.GetFd() != client.GetFd() && it->second.GetNickname() == params[0])
        {
            client.SendReply(":server 433 * " + params[0] + " :Nickname is already in use");
            return;
        }
    }
    client.SetNickname(params[0]);
    tryCompleteRegistration(client);
}

void handleUser(Server& server, Client& client, const std::vector<std::string>& params)
{
    (void)server;

    if(client.IsRegister())
    {
        client.SendReply(ERR_ALREADYREGISTRED(client.GetNickname()));
        return;
    }
    if (params.size() < 4)
    {
        client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "USER"));
        return;
    }
    if (!client.IsAuth())
    {
        client.SendReply(ERR_PASSWDMISMATCH(client.GetNickname()));
        return;
    }
    client.SetUsername(params[0]);
    client.SetHostname(params[1]);
    client.SetRealname(params[3]);
    tryCompleteRegistration(client);
}

void handleJoin(Server& server, Client& client, const std::vector<std::string>& params)
{
    if (params.empty())
    {
        client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "JOIN"));
        return;
    }

    std::string channelName = params[0];
    Channel* channel = server.GetChannel(channelName);

    if (!channel)
        channel = server.CreateChannel(channelName);

    if (channel->IsInviteOnly() && !channel->IsInvited(client.GetFd()))
    {
        client.SendReply(ERR_INVITEONLYCHAN(client.GetNickname(), channelName));
        return;
    }

    if (channel->HasKey())
    {
        std::string providedKey = (params.size() > 1) ? params[1] : "";
        if (providedKey != channel->GetKey())
        {
            client.SendReply(ERR_BADCHANNELKEY(client.GetNickname(), channelName));
            return;
        }
    }

    if (channel->GetUserLimit() > 0 &&
        (int)channel->GetMemberCount() >= channel->GetUserLimit())
    {
        client.SendReply(ERR_CHANNELISFULL(client.GetNickname(), channelName));
        return;
    }

    bool wasEmpty = (channel->GetMemberCount() == 0);
    channel->AddMember(&client);
    if (wasEmpty)
        channel->AddOperator(client.GetFd());
    std::string joinMsg = ":" + client.GetNickname() + "!" + client.GetUsername() +
                           "@" + client.GetIPaddr() + " JOIN " + channelName;
    channel->Broadcast(joinMsg);
}

void handlePart(Server& server, Client& client, const std::vector<std::string>& params)
{
    if (params.empty())
    {
        client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "PART"));
        return;
    }

    std::string channelName = params[0];
    Channel* channel = server.GetChannel(channelName);

    if (!channel)
    {
        client.SendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    if (!channel->IsMember(client.GetFd()))
    {
        client.SendReply(ERR_NOTONCHANNEL(channelName));
        return;
    }

    std::string partMsg = ":" + client.GetNickname() + "!" + client.GetUsername() +
                           "@" + client.GetIPaddr() + " PART " + channelName;
    channel->Broadcast(partMsg);
    channel->RemoveMember(client.GetFd());
}

void handleKick(Server& server, Client& client, const std::vector<std::string>& params)
{
    if (params.size() < 2)
    {
        client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "KICK"));
        return;
    }

    std::string channelName = params[0];
    std::string targetNick  = params[1];
    std::string reason      = (params.size() > 2) ? params[2] : "Kicked";

    Channel* channel = server.GetChannel(channelName);
    if (!channel)
    {
        client.SendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    if (!channel->IsOperator(client.GetFd()))
    {
        client.SendReply(ERR_CHANOPRIVSNEEDED(channelName));
        return;
    }

    Client* target = NULL;
    std::map<int, Client>::iterator it;
    for (it = server.GetClientsMap().begin(); it != server.GetClientsMap().end(); ++it)
    {
        if (it->second.GetNickname() == targetNick)
        {
            target = &(it->second);
            break;
        }
    }

    if (!target || !channel->IsMember(target->GetFd()))
    {
        client.SendReply(":server 441 " + targetNick + " " + channelName + " :They aren't on that channel");
        return;
    }

    std::string kickMsg = ":" + client.GetNickname() + "!" + client.GetUsername() +
                           "@" + client.GetIPaddr() + " KICK " + channelName + " " +
                           targetNick + " :" + reason;
    channel->Broadcast(kickMsg);
    channel->RemoveMember(target->GetFd());
}

void handlePrivmsg(Server& server, Client& client, const std::vector<std::string>& params)
{
    if (params.size() < 2)
    {
        client.SendReply(ERR_NEEDMOREPARAMS("PRIVMSG"));
        return;
    }

    std::string target  = params[0];
    std::string message = params[1];
    std::string fullMsg = ":" + client.GetNickname() + "!" + client.GetUsername() +
                           "@" + client.GetIPaddr() + " PRIVMSG " + target + " :" + message;

    if (target[0] == '#')
    {
        Channel* channel = server.GetChannel(target);
        if (!channel)
        {
            client.SendReply(ERR_NOSUCHCHANNEL(target));
            return;
        }
        channel->Broadcast(fullMsg, client.GetFd());
    }
    else
    {
        std::map<int, Client>::iterator it;
        for (it = server.GetClientsMap().begin(); it != server.GetClientsMap().end(); ++it)
        {
            if (it->second.GetNickname() == target)
            {
                it->second.SendReply(fullMsg);
                return;
            }
        }
        client.SendReply(":server 401 " + target + " :No such nick");
    }
}

void handleTopic(Server& server, Client& client, const std::vector<std::string>& params)
{
    if (params.empty())
    {
        client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "TOPIC"));
        return;
    }

    std::string channelName = params[0];
    Channel* channel = server.GetChannel(channelName);
    if (!channel)
    {
        client.SendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    if (!channel->IsMember(client.GetFd()))
    {
        client.SendReply(ERR_NOTONCHANNEL(channelName));
        return;
    }

    // Se apenas 1 param: consultar o topic atual
    if (params.size() == 1)
    {
        const std::string& t = channel->GetTopic();
        if (t.empty())
            client.SendReply(":server 331 " + client.GetNickname() + " " + channelName + " :No topic is set");
        else
            client.SendReply(":server 332 " + client.GetNickname() + " " + channelName + " :" + t);
        return;
    }

    // Alterar topic: se +t, apenas ops podem
    if (channel->IsTopicLocked() && !channel->IsOperator(client.GetFd()))
    {
        client.SendReply(ERR_CHANOPRIVSNEEDED(channelName));
        return;
    }

    std::string topic = params[1];
    channel->SetTopic(topic);
    std::string topicMsg = ":" + client.GetNickname() + "!" + client.GetUsername() +
                           "@" + client.GetIPaddr() + " TOPIC " + channelName + " :" + topic;
    channel->Broadcast(topicMsg);
}

void handleInvite(Server& server, Client& client, const std::vector<std::string>& params)
{
    if (params.size() < 2)
    {
        client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "INVITE"));
        return;
    }

    const std::string& targetNick = params[0];
    const std::string& channelName = params[1];

    Channel* channel = server.GetChannel(channelName);
    if (!channel)
    {
        client.SendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    if (!channel->IsMember(client.GetFd()))
    {
        client.SendReply(ERR_NOTONCHANNEL(channelName));
        return;
    }
    if (!channel->IsOperator(client.GetFd()))
    {
        client.SendReply(ERR_CHANOPRIVSNEEDED(channelName));
        return;
    }

    // Encontrar o target pelo nickname
    Client* target = NULL;
    std::map<int, Client>& clients = server.GetClientsMap();
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.GetNickname() == targetNick)
        {
            target = &(it->second);
            break;
        }
    }
    if (!target)
    {
        client.SendReply(":server 401 " + client.GetNickname() + " " + targetNick + " :No such nick");
        return;
    }
    if (channel->IsMember(target->GetFd()))
    {
        client.SendReply(":server 443 " + client.GetNickname() + " " + targetNick + " " + channelName + " :is already on channel");
        return;
    }

    channel->Invite(target->GetFd());
    // Confirmar ao invitado
    target->SendReply(":" + client.GetNickname() + "!" + client.GetUsername() +
                      "@" + client.GetIPaddr() + " INVITE " + targetNick + " :" + channelName);
    // RPL_INVITING ao operador
    client.SendReply(":server 341 " + client.GetNickname() + " " + targetNick + " " + channelName);
}

void handleQuit(Server& server, Client& client, const std::vector<std::string>& params)
{
    std::string reason = params.empty() ? "Quit" : params[0];
    std::string quitMsg = ":" + client.GetNickname() + "!" + client.GetUsername() +
                          "@" + client.GetIPaddr() + " QUIT :" + reason;

    // Notificar todos os canais onde o cliente está
    std::map<std::string, Channel>& channels = server.GetChannelsMap();
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        Channel& ch = it->second;
        if (ch.IsMember(client.GetFd()))
        {
            ch.Broadcast(quitMsg, client.GetFd());
            ch.RemoveMember(client.GetFd());
        }
    }
    client.SendReply(":server ERROR :Closing Link: " + reason);
    server.ClearClients(client.GetFd());
}

void handleMode(Server& server, Client& client, const std::vector<std::string>& params)
{
    if (params.empty())
    {
        client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "MODE"));
        return;
    }

    const std::string& channelName = params[0];
    Channel* channel = server.GetChannel(channelName);
    if (!channel)
    {
        client.SendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }

    // MODE #chan sem mais args: devolve modos ativos
    if (params.size() == 1)
    {
        std::string modes = "+";
        if (channel->IsInviteOnly())  modes += "i";
        if (channel->IsTopicLocked()) modes += "t";
        if (channel->HasKey())        modes += "k";
        if (channel->GetUserLimit() > 0) modes += "l";
        client.SendReply(":server 324 " + client.GetNickname() + " " + channelName + " " + modes);
        return;
    }

    if (!channel->IsOperator(client.GetFd()))
    {
        client.SendReply(ERR_CHANOPRIVSNEEDED(channelName));
        return;
    }

    const std::string& modeStr = params[1];
    size_t paramIdx = 2; // index nos params extra (key, nick, limit)
    char sign = '+';

    for (size_t i = 0; i < modeStr.size(); ++i)
    {
        char m = modeStr[i];
        if (m == '+' || m == '-') { sign = m; continue; }

        if (m == 'i')
        {
            channel->SetInviteOnly(sign == '+');
            channel->Broadcast(":" + client.GetNickname() + "!" + client.GetUsername()
                + "@" + client.GetIPaddr() + " MODE " + channelName
                + " " + sign + "i");
        }
        else if (m == 't')
        {
            channel->SetTopicLocked(sign == '+');
            channel->Broadcast(":" + client.GetNickname() + "!" + client.GetUsername()
                + "@" + client.GetIPaddr() + " MODE " + channelName
                + " " + sign + "t");
        }
        else if (m == 'k')
        {
            if (sign == '+')
            {
                if (paramIdx >= params.size())
                {
                    client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "MODE +k"));
                    continue;
                }
                channel->SetKey(params[paramIdx]);
                channel->Broadcast(":" + client.GetNickname() + "!" + client.GetUsername()
                    + "@" + client.GetIPaddr() + " MODE " + channelName
                    + " +k " + params[paramIdx]);
                ++paramIdx;
            }
            else
            {
                channel->RemoveKey();
                channel->Broadcast(":" + client.GetNickname() + "!" + client.GetUsername()
                    + "@" + client.GetIPaddr() + " MODE " + channelName + " -k");
            }
        }
        else if (m == 'o')
        {
            if (paramIdx >= params.size())
            {
                client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "MODE +o"));
                continue;
            }
            const std::string& targetNick = params[paramIdx++];
            Client* target = NULL;
            std::map<int, Client>& clients = server.GetClientsMap();
            for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
            {
                if (it->second.GetNickname() == targetNick)
                { target = &(it->second); break; }
            }
            if (!target || !channel->IsMember(target->GetFd()))
            {
                client.SendReply(":server 441 " + client.GetNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel");
                continue;
            }
            if (sign == '+') channel->AddOperator(target->GetFd());
            else             channel->RemoveOperator(target->GetFd());
            channel->Broadcast(":" + client.GetNickname() + "!" + client.GetUsername()
                + "@" + client.GetIPaddr() + " MODE " + channelName
                + " " + sign + "o " + targetNick);
        }
        else if (m == 'l')
        {
            if (sign == '+')
            {
                if (paramIdx >= params.size())
                {
                    client.SendReply(ERR_NEEDMOREPARAMS(client.GetNickname(), "MODE +l"));
                    continue;
                }
                int limit = std::atoi(params[paramIdx].c_str());
                if (limit <= 0)
                {
                    client.SendReply(":server 501 " + client.GetNickname() + " :Invalid limit");
                    ++paramIdx;
                    continue;
                }
                channel->SetUserLimit(limit);
                channel->Broadcast(":" + client.GetNickname() + "!" + client.GetUsername()
                    + "@" + client.GetIPaddr() + " MODE " + channelName
                    + " +l " + params[paramIdx]);
                ++paramIdx;
            }
            else
            {
                channel->SetUserLimit(0);
                channel->Broadcast(":" + client.GetNickname() + "!" + client.GetUsername()
                    + "@" + client.GetIPaddr() + " MODE " + channelName + " -l");
            }
        }
        else
        {
            client.SendReply(":server 472 " + client.GetNickname() + " " + m + " :is unknown mode char to me");
        }
    }
}

void dispatch(Server& server, Client& client, const IRCMessage& msg)
{
    if (!client.IsRegister() && msg.command != "PASS" && msg.command != "NICK" && msg.command != "USER")
    {
        client.SendReply(ERR_NOTREGISTERED(client.GetNickname()));
        return;
    }

    if (msg.command == "PASS")
        handlePass(server, client, msg.params);
    else if (msg.command == "NICK")
        handleNick(server, client, msg.params);
    else if (msg.command == "USER")
        handleUser(server, client, msg.params);
    else if (msg.command == "JOIN")
        handleJoin(server, client, msg.params);
    else if (msg.command == "PART")
        handlePart(server, client, msg.params);
    else if (msg.command == "PRIVMSG")
        handlePrivmsg(server, client, msg.params);
    else if (msg.command == "KICK")
        handleKick(server, client, msg.params);
    else if (msg.command == "TOPIC")
        handleTopic(server, client, msg.params);
    else if (msg.command == "INVITE")
        handleInvite(server, client, msg.params);
    else if (msg.command == "QUIT")
        handleQuit(server, client, msg.params);
    else if (msg.command == "MODE")
        handleMode(server, client, msg.params);
    else
        client.SendReply(ERR_UNKNOWNCOMMAND(msg.command));
}