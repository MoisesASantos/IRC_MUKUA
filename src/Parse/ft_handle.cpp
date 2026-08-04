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
}

void handleUser(Server& server, Client& client, const std::vector<std::string>& params)
{
    (void)server;
    if (params.size() < 4)
    {
        client.SendReply(ERR_NEEDMOREPARAMS("USER"));
        return;
    }
    client.SetUsername(params[0]);
    client.SetHostname(params[1]);
    client.SetRealname(params[3]);
    client.SetRegister(true);
}

void handleJoin(Server& server, Client& client, const std::vector<std::string>& params)
{
    if (params.empty())
    {
        client.SendReply(ERR_NEEDMOREPARAMS("JOIN"));
        return;
    }

    std::string channelName = params[0];
    Channel* channel = server.GetChannel(channelName);

    if (!channel)
        channel = server.CreateChannel(channelName);

    if (channel->IsInviteOnly() && !channel->IsInvited(client.GetFd()))
    {
        client.SendReply(":server 473 " + channelName + " :Cannot join channel (+i)");
        return;
    }

    if (channel->HasKey())
    {
        std::string providedKey = (params.size() > 1) ? params[1] : "";
        if (providedKey != channel->GetKey())
        {
            client.SendReply(":server 475 " + channelName + " :Cannot join channel (+k)");
            return;
        }
    }

    if (channel->GetUserLimit() > 0 &&
        (int)channel->GetMemberCount() >= channel->GetUserLimit())
    {
        client.SendReply(":server 471 " + channelName + " :Cannot join channel (+l)");
        return;
    }

    bool wasEmpty = (channel->GetMemberCount() == 0);
    channel->AddMember(&client);
    if (wasEmpty)
        channel->AddOperator(client.GetFd()); // primeiro a entrar vira operador

    std::string joinMsg = ":" + client.GetNickname() + "!" + client.GetUsername() +
                           "@" + client.GetIPaddr() + " JOIN " + channelName;
    channel->Broadcast(joinMsg);
}

void handlePart(Server& server, Client& client, const std::vector<std::string>& params)
{
    if (params.empty())
    {
        client.SendReply(ERR_NEEDMOREPARAMS("PART"));
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
        client.SendReply(ERR_NEEDMOREPARAMS("KICK"));
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

void dispatch(Server& server, Client& client, const IRCMessage& msg)
{
    if (msg.command == "NICK")
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
    else
        client.SendReply(ERR_UNKNOWNCOMMAND(msg.command));
}