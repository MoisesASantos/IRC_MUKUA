/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 15:45:52 by mosantos          #+#    #+#             */
/*   Updated: 2026/08/06 19:07:13 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Headers/header.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: /ircserv <port> <password>" << std::endl;
        return 1;
    }
    Server server;
    std::string port = argv[1];
    std::string password = argv[2];
    
    if (port.size() == 0 || password.size() == 0 )
    {
        std::cout << "Usage: /ircserv <port> <password>" << std::endl;
        return 1;
    }
    if (password.find(' ') != std::string::npos)
    {
        std::cerr << "Error: password cannot contain spaces\n";
        return 1;
    }
    server.setPassword(argv[2]);
    signal(SIGINT, Server::SignalHandler);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, Server::SignalHandler);
    ft_execute_server(server, argv[1]);
	return 0;
}
