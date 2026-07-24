/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 14:10:11 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/24 12:31:21 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../Headers/includes.hpp"
#include "../Client/Client.hpp"

class Server
{
	private:
		int	_port;
		int _serSocketFd;
		static bool _signal;
		std::string	_password;
		std::vector<pollfd> _fds;
		std::vector<Client> _clients;
	
	public:
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();
	
		void ServerInit(std::string port);
		void SerSocket();
		void AcceptNewClient();
		void ReceiveNewData(int fd);
	
		static void SignalHandler(int signum);
	
		void CloseFds();
		void ClearClients(int fd);
};
