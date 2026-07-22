/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:40:39 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/22 14:04:32 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <ostream>

class Client
{
	private:
		int	Fd;
		std::string	IPaddr;
	
		public:
		Client();
		Client(const Client& other);
		Client& operator=(const Client& other);
		~Client();

		
		int GetFd() const;
		void GetIPaddr() const;
		void SetFd(int fd);
		void SetIPaddr(std::string ipaddr);
};
