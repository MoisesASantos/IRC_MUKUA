/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 14:10:20 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/27 15:16:46 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : _serSocketFd(-1) {}
	
Server::Server(const Server& other) 
	: 
	_port(other._port), 
	_serSocketFd(other._serSocketFd),
	_password(other._password),
	_epollFd(other._epollFd),
	_clients(other._clients)
{	
}

Server& Server::operator=(const Server& other) {
	
	if (this != &other) {
		
		this->_port = other._port;
		this->_serSocketFd = other._serSocketFd;
		this->_clients = other._clients;
		this->_epollFd = other._epollFd;
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
	if (!(_port > 0 && _port <= 65535))
	{
		std::cout << "You should use a valid port" << std::endl;
		return ;
	}
	try
	{
		_epollFd = epoll_create1(0);
		SerSocket();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(1);
	}
	std::cout << GRE << "Server <" << _serSocketFd << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";
}

void Server::SerSocket()
{
	sockaddr_in add;
	epoll_event event;
	
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
	event.events = EPOLLIN;
	event.data.fd = _serSocketFd;
	epoll_ctl(_epollFd, EPOLL_CTL_ADD, _serSocketFd, &event);
}

void Server::AcceptNewClient()
{
	Client client;
	sockaddr_in cliadd;
	char ip[INET_ADDRSTRLEN];
	socklen_t len = sizeof(cliadd);
	epoll_event event;

	int connecfd = accept(_serSocketFd, (sockaddr *)&(cliadd), &len);
	if (connecfd == -1)
	{
		std::cout << "accept() failed" << std::endl;
		return;
	}
	if (fcntl(connecfd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "fcntl() failed" << std::endl;
		return;
	}

	event.events = EPOLLIN;
	event.data.fd = connecfd;
	inet_ntop(AF_INET, &cliadd.sin_addr, ip, INET_ADDRSTRLEN);

	client.SetIPaddr(ip);
	client.SetFd(connecfd);
	_clients[connecfd] = client;
	epoll_ctl(_epollFd, EPOLL_CTL_ADD, connecfd, &event);
	std::cout << GRE << "Client <" << connecfd << "> Connected" << WHI << std::endl;
}

void Server::ReceiveNewData(Client& client)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(client.GetFd(), buff, sizeof(buff) - 1, 0);
	if (bytes <= 0)
	{
		std::cout << RED << "Client <" << client.GetFd() << "> Disconnected" << WHI << std::endl;
    	ClearClients(client.GetFd());
		return;
	}
	
    buff[bytes] = '\0';
	std::cout << YEL << "Client <" << client.GetFd() << "> Data: " << WHI << buff;
    client.AppendBuffer(std::string(buff, bytes));
}

bool Server::_signal = false;
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::_signal = true;
}

bool Server::IsRunning()
{
	return !_signal;
}

void Server::CloseFds()
{
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end();++it)
    {
        std::cout << RED << "Client <" << it->first << "> Disconnected" << WHI << std::endl;
        close(it->first);
    }
    _clients.clear();

    if (_serSocketFd != -1)
    {
        std::cout << RED << "Server <" << _serSocketFd << "> Disconnected" << WHI << std::endl;
		close(_serSocketFd);
        _serSocketFd = -1;
    }
    if (_epollFd != -1)
    {
        close(_epollFd);
        _epollFd = -1;
    }
}

void Server::ClearClients(int fd)
{
    if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1)
	{
    	std::cerr << "epoll_ctl DEL failed" << std::endl;
	}
    close(fd);
    _clients.erase(fd);
}

//Getters
int Server::GetServerSocketFd() const
{
    return _serSocketFd;
}

const std::string& Server::GetPassword() const
{
    return _password;
}

Client* Server::GetClient(int fd)
{
    std::map<int, Client>::iterator it = _clients.find(fd);

    if (it == _clients.end())
        return NULL;

    return &(it->second);
}

int Server::GetEpollFd() const
{
    return _epollFd;
}

//Setters
void	Server::setPassword(std::string pass) {
	this->_password = pass;
}

std::map<int, Client>& Server::GetClientsMap()
{
    return _clients;
}

Channel* Server::GetChannel(const std::string& name)
{
    std::map<std::string, Channel>::iterator it = _channels.find(name);
    if (it == _channels.end())
        return NULL;
    return &(it->second);
}

Channel* Server::CreateChannel(const std::string& name)
{
    _channels[name] = Channel(name);
    return &_channels[name];
}

std::map<std::string, Channel>& Server::GetChannelsMap()
{
    return _channels;
}


