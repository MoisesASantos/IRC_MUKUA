/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:40:39 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/22 18:26:52 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../Headers/includes.hpp"

class Client
{
private:
	int	_fd;
	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _recvBuffer;
	std::string	_ipAddr;
	bool        _authenticated;
	bool        _registered;

public:
	Client();
	Client(const Client& other);
	Client& operator=(const Client& other);
	~Client();

	//Getters
	int						GetFd() const;
	const std::string&		GetNickname() const;
	const std::string&		GetHostname() const;
	const std::string&		GetUsername() const;
	const std::string&		GetIPaddr() const;
	const std::string& 		GetBuffer() const; 
	bool					IsAuth() const;
	bool					IsRegister() const;

	//Setters
	void 	SetFd(int fd);
	void 	SetIPaddr(const std::string& ipaddr);
	void	SetNickname(const std::string& nickname);
	void	SetHostname(const std::string& hostname);
	void	SetUsername(const std::string& username);
	void	SetAuth(bool value);
	void	SetRegister(bool value);

	//Others
	void	ClearBuffer();
	void AppendBuffer(const std::string& data);
	// bool Client::HasCompleteMessage() const;
	bool HasCompleteMessage() const;
	// std::string Client::ExtractMessage();
	std::string ExtractMessage();
};
