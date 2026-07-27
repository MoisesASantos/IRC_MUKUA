/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_engine.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 12:31:22 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/27 15:06:42 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Headers/header.hpp"


void    ft_execute_server(Server& server, std::string port)
{
    Client*     client;
    std::string command;
    int ret;
    pollfd& pfd;

    server.ServerInit(port);
    while (server.IsRunning())
    {
        ret = poll(&server.GetPollFd(0), server.GetFdCount(), 0);

        if (ret <= 0)
            continue;
        for (size_t i = 0; i < server.GetFdCount(); i++)
        {
            pfd = server.GetPollFd(i);
            
            if (pfd.revents & POLLIN)
                continue;
            if (pfd.fd == server.GetServerSocketFd())
                server.AcceptNewClient();
            else
            {
                client = server.GetClient(pfd.fd);
                if (client)
                {
                    server.ReceiveNewData(*(client));
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