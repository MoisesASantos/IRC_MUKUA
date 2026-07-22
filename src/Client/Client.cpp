/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:42:15 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/22 18:28:34 by mosantos         ###   ########.fr       */
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

const std::string& Client::GetBuffer() const {
	return (this->_recvBuffer);
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

//others
void Client::AppendBuffer(const std::string& data)
{
    this->_recvBuffer += data;
}

bool Client::HasCompleteMessage() const
{
    return (_recvBuffer.find("\r\n") != std::string::npos);
}

std::string Client::ExtractMessage()
{
    size_t pos = _recvBuffer.find("\r\n");

    if (pos == std::string::npos)
        return "";
    std::string message = _recvBuffer.substr(0, pos);
    _recvBuffer.erase(0, pos + 2);
    return message;
}

void Client::ClearBuffer() {
	_recvBuffer.clear();
}