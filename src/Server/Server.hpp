/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 14:10:11 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/27 15:13:43 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../Headers/includes.hpp"
#include "../Client/Client.hpp"
#include "../Channel/Channel.hpp"

class Server
{
	private:
		int	_port;
		int _serSocketFd;
		static bool _signal;
		std::string	_password;
		int _epollFd;
		std::map<int, Client> _clients;
		std::map<std::string, Channel> _channels;
	
	public:
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();
	
		//Getters
		int GetServerSocketFd() const;
		int GetEpollFd() const;
		const std::string& GetPassword() const;

		//Setters
		void	setPassword(std::string pass);
		
		//Server logic method
		void ServerInit(std::string port);
		void SerSocket();
		void AcceptNewClient();
		void ReceiveNewData(Client& client);

		//Signal Method
		static void SignalHandler(int signum);
		static bool IsRunning();

		//CleanUp Method
		void CloseFds();
		void ClearClients(int fd);
		Client* GetClient(int fd);
		std::map<int, Client>& GetClientsMap();
		Channel* GetChannel(const std::string& name);
		Channel* CreateChannel(const std::string& name);
};
