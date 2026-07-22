/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:42:15 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/22 14:04:25 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(){}

Client::Client(const Client& other) : Fd(other.Fd), IPaddr(other.IPaddr) {}

Client& Client::operator=(const Client& other) {
	
	if (this != &other)
	{
		this->Fd = other.Fd;
		this->IPaddr = other.IPaddr;
	}
	return *this;
}

Client::~Client(){}

int	Client::GetFd() const {
	return this->Fd;
}

void	Client::SetFd(int fd) {
	this->Fd = fd;
}

void Client::GetIPaddr() const {
	std::cout << this->IPaddr << std::endl;
}


void	Client::SetIPaddr(std::string ipaddr) {
	this->IPaddr = ipaddr;
}
