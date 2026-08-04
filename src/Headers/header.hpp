/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emjoao <emjoao@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 15:51:16 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/31 15:04:02 by emjoao           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "includes.hpp"
#include "../Client/Client.hpp"
#include "../Server/Server.hpp"

struct IRCMessage
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
};


void    ft_execute_server(Server& server, std::string port);
IRCMessage parseMessage(const std::string& raw);
void dispatch(Server& server, Client& client, const IRCMessage& msg);
void handleNick(Server& server, Client& client, const std::vector<std::string>& params);
void handleUser(Server& server, Client& client, const std::vector<std::string>& params);
void handleJoin(Server& server, Client& client, const std::vector<std::string>& params);
void handlePart(Server& server, Client& client, const std::vector<std::string>& params);
void handleKick(Server& server, Client& client, const std::vector<std::string>& params);
void handlePrivmsg(Server& server, Client& client, const std::vector<std::string>& params);