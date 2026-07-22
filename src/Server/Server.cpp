/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 14:10:20 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/22 18:29:20 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : SerSocketFd(-1) {}
	
Server::Server(const Server& other) : Port(other.Port), SerSocketFd(other.SerSocketFd), clients(other.clients), fds(other.fds) {}

Server& Server::operator=(const Server& other) {
	
	if (this != &other) {
		
		this->Port = other.Port;
		this->SerSocketFd = other.SerSocketFd;
		this->clients = other.clients;
		this->fds = other.fds;
	}
	return *this;
}

Server::~Server() {}


void Server::ServerInit()
{
	this->Port = 4444;
	SerSocket();
	std::cout << GRE << "Server <" << SerSocketFd << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";
}

void Server::SerSocket()
{
	sockaddr_in add;
	pollfd NewPoll;
	
	add.sin_family = AF_INET;
	add.sin_port = htons(this->Port);
	add.sin_addr.s_addr = INADDR_ANY;

	SerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(SerSocketFd == -1)
		throw(std::runtime_error("faild to create socket"));

	int en = 1;
	if(setsockopt(SerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	if (fcntl(SerSocketFd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	if (bind(SerSocketFd, (sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	if (listen(SerSocketFd, SOMAXCONN) == -1)
		throw(std::runtime_error("listen() faild"));

	NewPoll.fd = SerSocketFd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;
	fds.push_back(NewPoll);
}

void Server::AcceptNewClient()
{
	Client cli;
	sockaddr_in cliadd;
	pollfd NewPoll;
	socklen_t len = sizeof(cliadd);

	int incofd = accept(SerSocketFd, (sockaddr *)&(cliadd), &len);
	if (incofd == -1)
		{std::cout << "accept() failed" << std::endl; return;}

	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
		{std::cout << "fcntl() failed" << std::endl; return;}

	NewPoll.fd = incofd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	cli.SetFd(incofd);
	cli.SetIPaddr(inet_ntoa((cliadd.sin_addr)));
	clients.push_back(cli);
	fds.push_back(NewPoll);
	std::cout << GRE << "Client <" << incofd << "> Connected" << WHI << std::endl;
}

void Server::ReceiveNewData(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);
	if(bytes <= 0){
		
		std::cout << RED << "Client <" << fd << "> Disconnected" << WHI << std::endl;
		ClearClients(fd);
		close(fd);
	} else{
		
		buff[bytes] = '\0';
		std::cout << YEL << "Client <" << fd << "> Data: " << WHI << buff;	
	}
}

bool Server::Signal = false;
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::Signal = true;
}

void Server::CloseFds(){
	
	for(size_t i = 0; i < clients.size(); i++) {
		std::cout << RED << "Client <" << clients[i].GetFd() << "> Disconnected" << WHI << std::endl;
		close(clients[i].GetFd());
	}
	if (SerSocketFd != -1) {
		std::cout << RED << "Server <" << SerSocketFd << "> Disconnected" << WHI << std::endl;
		close(SerSocketFd);
	}
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
