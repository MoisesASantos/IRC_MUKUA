/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emjoao <emjoao@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 15:45:52 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/31 11:26:12 by emjoao           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Headers/header.hpp"

int main(int argc, char **argv)
{

    IRCMessage m1 = parseMessage("NICK edman");
    IRCMessage m2 = parseMessage("USER edman 0 * :Edman Real Name");
    IRCMessage m3 = parseMessage(":edman!edman@localhost KICK #general moises :saiu por regras");

    std::cout << "m1.command=" << m1.command << std::endl;
    std::cout << "m2.command=" << m2.command << std::endl;
    std::cout << "m3.prefix=" << m3.prefix << " command=" << m3.command << std::endl;
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
