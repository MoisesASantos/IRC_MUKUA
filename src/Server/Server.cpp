/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 14:10:20 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/22 14:36:13 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : SerSocketFd(-1) {}
	
Server::Server(const Server& other) : Port(other.Port), SerSocketFd(other.SerSocket), clients(other.clients), fds(other.fds) {}

Server& operator=(const Server& other) {
	
	if (this != &other) {
		
		this->Port = other.Port;
		this->SerSocketFd = other.SerSocketFd;
		this->clients = other.clients;
		this->fds = other.fds;
	}
	return *this;
}

Server::~Server() {}


void Server::ServerInit() {
	
}

void Server::SerSocket() {
	
}


void Server::AcceptNewClient() {
	
}

void Server::ReceiveNewData(int fd) {
	
}

void Server::SignalHandler(int signum) {
	
}

void Server::CloseFds() {
	
}

void Server::ClearClients(int fd) {
	
	for(size_t i = 0; i < fds.size(); i++) {
		
		if (fds[i].fd == fd) {	
			fds.erase(fds.begin() + i); 
			break;
		}
	}
	for(size_t i = 0; i < clients.size(); i++) {
		
		if (clients[i].GetFd() == fd) {
			clients.erase(clients.begin() + i);
			break;
		}
	}
}
