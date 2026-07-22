/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:42:15 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/22 17:47:09 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : _fd(-1), _authenticated(false), _registered(false) {}

Client::Client(const Client& other)
    : _fd(other._fd),
      _nickname(other._nickname),
      _username(other._username),
      _hostname(other._hostname),
      _recvBuffer(other._recvBuffer),
      _ipAddr(other._ipAddr),
      _authenticated(other._authenticated),
      _registered(other._registered)
{
}

Client& Client::operator=(const Client& other) {
	
	if (this != &other)
	{
		this->_fd				=	other._fd;
		this->_nickname			=	other._nickname;
    	this->_username			=	other._username;
    	this->_hostname			=	other._hostname;
		this->_recvBuffer		=	other._recvBuffer;
		this->_ipAddr 			=	other._ipAddr;
    	this->_authenticated	= 	other._authenticated;
    	this->_registered		= 	other._registered;
	}
	return *this;
}

Client::~Client(){}

//Getters
int	Client::GetFd() const {
	return (this->_fd);
}

const std::string& Client::GetIPaddr() const {
	return (this->_ipAddr);
}

const std::string&	Client::GetNickname() const {
	return (this->_nickname);
}

const std::string&	Client::GetHostname() const {
	return (this->_hostname);
}

const std::string&	Client::GetUsername() const {
	return (this->_username);
}

bool			Client::IsAuth() const {
	return (this->_authenticated);	
}

bool			Client::IsRegister() const {
	return (this->_registered);
}

//Setters
void	Client::SetFd(int fd) {
	this->_fd = fd;
}

void	Client::SetIPaddr(const std::string& ipaddr) {
	this->_ipAddr = ipaddr;
}

void	Client::SetNickname(const std::string& nickname) {
	this->_nickname = nickname;
}

void	Client::SetHostname(const std::string& hostname) {
	this->_hostname = hostname;
}

void	Client::SetUsername(const std::string& username) {
	this->_username = username;
}

void	Client::SetAuth(bool value) {
	this->_authenticated = value;
}

void	Client::SetRegister(bool value) {
	this->_registered = value;
}