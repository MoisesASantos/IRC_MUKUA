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
	if (!(_port > 0 && _port <= 65535))
	{
		std::cout << "You should use a valid port" << std::endl;
		return ;
	}
	try
	{
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
	Client client;
	sockaddr_in cliadd;
	pollfd NewPoll;
	char ip[INET_ADDRSTRLEN];
	socklen_t len = sizeof(cliadd);

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

	NewPoll.fd = connecfd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	inet_ntop(AF_INET, &cliadd.sin_addr, ip, INET_ADDRSTRLEN);

	client.SetIPaddr(ip);
	client.SetFd(connecfd);
	_clients[connecfd] = client;
	_fds.push_back(NewPoll);
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
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
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
    _fds.clear();
}

void Server::ClearClients(int fd)
{
    for (size_t i = 0; i < _fds.size(); i++)
    {
        if (_fds[i].fd == fd)
        {
            _fds.erase(_fds.begin() + i);
            break;
        }
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

size_t Server::GetFdCount() const
{
    return _fds.size();
}

pollfd& Server::GetPollFd(size_t index)
{
    return _fds[index];
}

//Setters
void	Server::setPassword(std::string pass) {
	this->_password = pass;
}
