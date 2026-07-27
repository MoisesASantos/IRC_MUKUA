/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 15:45:52 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/27 14:09:49 by mosantos         ###   ########.fr       */
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
    
    if (port.size() == 0 || password.size() == 0)
    {
        std::cout << "Usage: /ircserv <port> <password>" << std::endl;
        return 1;
    }
    server.setPassword(argv[2]);
    ft_execute_server(server, argv[1]);
	return 0;
}
