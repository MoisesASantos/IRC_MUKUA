/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 14:10:20 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/24 15:33:22 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : _serSocketFd(-1) {}
	
Server::Server(const Server& other) 
	: 
	_port(other._port), 
	_serSocketFd(other._serSocketFd),
	_password(other._password),
	_fds(other._fds),
	_clients(other._clients)
{	
}

Server& Server::operator=(const Server& other) {
	
	if (this != &other) {
		
		this->_port = other._port;
		this->_serSocketFd = other._serSocketFd;
		this->_clients = other._clients;
		this->_fds = other._fds;
		this->_password = other._password;
	}
	return *this;
}

Server::~Server() {}


void Server::ServerInit(std::string port)
{
	int Port;
	std::stringstream ss(port);

	if (ss >> Port) {
		this->_port = Port;
	} else {
		std::cout << "You should use a valid port" << std::endl;
		return ;
	}
	if (_port > 0 && _port <= 65535)
	{
		std::cout << "You should use a valid port" << std::endl;
		return ;
	}
	SerSocket();
	std::cout << GRE << "Server <" << _serSocketFd << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";
}

void Server::SerSocket()
{
	sockaddr_in add;
	pollfd NewPoll;
	
	add.sin_family = AF_INET;
	add.sin_port = htons(this->_port);
	add.sin_addr.s_addr = INADDR_ANY;

	_serSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(_serSocketFd == -1)
		throw(std::runtime_error("faild to create socket"));

	int enable = 1;
	//in this line we change the socket config, to enable reuse the address, without have a error after finish the server
	if(setsockopt(_serSocketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	//in this line we use fcntl to change the file descriptor config, to be non-blocking this way the process non-block request for other clients
	if (fcntl(_serSocketFd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	//in this line we bind, to config the identify of our socker, who port and address
	if (bind(_serSocketFd, (sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	//in this we start the listen, for socket
	if (listen(_serSocketFd, SOMAXCONN) == -1)
		throw(std::runtime_error("listen() faild"));

	//we create a poll to monitore the events on file descriptor, in our case, the socket
	NewPoll.fd = _serSocketFd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;
	_fds.push_back(NewPoll);
}

void Server::AcceptNewClient()
{
	Client cli;
	sockaddr_in cliadd;
	pollfd NewPoll;
	socklen_t len = sizeof(cliadd);

	int incofd = accept(_serSocketFd, (sockaddr *)&(cliadd), &len);
	if (incofd == -1)
		{std::cout << "accept() failed" << std::endl; return;}

	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
		{std::cout << "fcntl() failed" << std::endl; return;}

	NewPoll.fd = incofd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	cli.SetFd(incofd);
	cli.SetIPaddr(inet_ntoa((cliadd.sin_addr)));
	_clients.push_back(cli);
	_fds.push_back(NewPoll);
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

bool Server::_signal = false;
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::_signal = true;
}

void Server::CloseFds(){
	
	for(size_t i = 0; i < _clients.size(); i++) {
		std::cout << RED << "Client <" << _clients[i].GetFd() << "> Disconnected" << WHI << std::endl;
		close(_clients[i].GetFd());
	}
	if (_serSocketFd != -1) {
		std::cout << RED << "Server <" << _serSocketFd << "> Disconnected" << WHI << std::endl;
		close(_serSocketFd);
	}
}

void Server::ClearClients(int fd) {
	
	for(size_t i = 0; i < _fds.size(); i++) {
		
		if (_fds[i].fd == fd) {	
			_fds.erase(_fds.begin() + i); 
			break;
		}
	}
	for(size_t i = 0; i < _clients.size(); i++) {
		
		if (_clients[i].GetFd() == fd) {
			_clients.erase(_clients.begin() + i);
			break;
		}
	}
}
