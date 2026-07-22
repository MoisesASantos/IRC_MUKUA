/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 14:10:11 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/22 18:22:18 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../Headers/includes.hpp"
#include "../Client/Client.hpp"

class Server
{
	private:
		int	Port;
		int SerSocketFd;
		static bool Signal;
		std::vector<Client> clients;
		std::vector<pollfd> fds;
	
	public:
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();
	
		void ServerInit();
		void SerSocket();
		void AcceptNewClient();
		void ReceiveNewData(int fd);
	
		static void SignalHandler(int signum);
	
		void CloseFds();
		void ClearClients(int fd);
};
