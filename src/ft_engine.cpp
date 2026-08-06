/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_engine.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 12:31:22 by mosantos          #+#    #+#             */
/*   Updated: 2026/08/06 19:05:57 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Headers/header.hpp"
#include "./Server/ServerHandler.hpp"
#include <exception>


void ft_execute_server(Server& server, std::string port)
{
    Client* client;
    std::string command;
    epoll_event events[64];
    int ready;

    try {
        server.ServerInit(port);
    } catch (const std::exception& err){
        std::cout << "Erro :" << err.what() << std::endl;
        return ;
    }

    while (server.IsRunning())
    {
        ServerHandler handler;
        handler.setServer(&server);
        ready = epoll_wait(server.GetEpollFd(), events, 64, -1);
        if (ready == -1)
        {
            if (errno == EINTR)
                continue;
            break;
        }

        for (int i = 0; i < ready; i++)
        {
            int fd = events[i].data.fd;

            if (fd == server.GetServerSocketFd())
            {
                server.AcceptNewClient();
            }
            else
            {
                client = server.GetClient(fd);

                if (!client)
                    continue;
                server.ReceiveNewData(*client);
                while (client->HasCompleteMessage())
                {
                    command = client->ExtractMessage();
                    handler.processCommand(client, command);
                }
            }
        }
    }
    server.CloseFds();
}
