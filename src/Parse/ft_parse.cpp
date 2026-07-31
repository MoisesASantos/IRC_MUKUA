/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_parse.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emjoao <emjoao@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 09:24:38 by emjoao            #+#    #+#             */
/*   Updated: 2026/07/31 12:30:29 by emjoao           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Headers/header.hpp"

void ExtractParams(const std::string& raw, size_t& pos, std::vector<std::string>& params)
{
    while (pos < raw.size())
    {
        if (raw[pos] == ':')
        {
            params.push_back(raw.substr(pos + 1));
            return;
        }

        size_t space = raw.find(' ', pos);
        if (space == std::string::npos)
        {
            params.push_back(raw.substr(pos));
            return;
        }
        params.push_back(raw.substr(pos, space - pos));
        pos = space + 1;
    }
}

std::string ExtractPrefix(const std::string& raw, size_t& pos)
{
    std::string msg;

    pos = raw.find(' ', pos);
    if(pos != std::string::npos)
    {
        msg = raw.substr(1, pos - 1);
        pos++;
    }
    return (msg);
}

std::string ExtractCommand(const std::string& raw, size_t& pos)
{
    std::string command;
    size_t start = pos;
    size_t space = raw.find(' ', pos);

    if (space == std::string::npos)
    {
        command = raw.substr(start);
        pos = raw.size();
    }
    else
    {
        command = raw.substr(start, space - start);
        pos = space + 1;
    }
    return (command);
}

IRCMessage parseMessage(const std::string& raw)
{
    IRCMessage msg;
    size_t pos = 0;

    if (raw[0] == ':')
        msg.prefix = ExtractPrefix(raw, pos);
    msg.command = ExtractCommand(raw, pos);
    ExtractParams(raw, pos, msg.params);
    return (msg);
}