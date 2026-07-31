/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emjoao <emjoao@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 15:51:16 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/31 09:54:06 by emjoao           ###   ########.fr       */
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