/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_engine.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 12:31:22 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/27 13:53:15 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Headers/header.hpp"


void    ft_execute_server(Server& server, std::string port)
{
    Client*     client;
    std::string command;

    server.ServerInit(port);
    while (server.IsRunning())
    {
        for (size_t i = 0; i < _fds.size(); i++)
        {
            if (!(_fds[i].revents & POLLIN))
                continue;

            if (_fds[i].fd == server.GetServerSocketFd())
            {
                server.AcceptNewClient();
            }
            else
            {
                client = GetClient(fd);
                if (client)
                {
                    server.ReceiveNewData(client);
                    if (client->HasCompleteMessage())
                    {
                        command = client->ExtractMessage();
                    }
                }
            }
        }
    }
    server.CloseFds();
}