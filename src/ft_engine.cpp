/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_engine.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emjoao <emjoao@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 12:31:22 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/31 12:39:41 by emjoao           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Headers/header.hpp"


void ft_execute_server(Server& server, std::string port)
{
    Client* client;
    std::string command;
    epoll_event events[64];
    int ready;

    server.ServerInit(port);

    while (server.IsRunning())
    {
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
                    IRCMessage msg = parseMessage(command);
                    dispatch(server, *client, msg);
                }
            }
        }
    }
    server.CloseFds();
}
