/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgaspar <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 15:19:43 by sgaspar           #+#    #+#             */
/*   Updated: 2026/08/06 15:19:43 by sgaspar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Bot {
private:
	int _fd;
	std::string _ip;
	int _port;
	std::string _password;

public:
	Bot();
	Bot(const std::string& ip, int port, const std::string& password);
	Bot(const Bot& other);
	Bot& operator=(const Bot& other); 
	~Bot();

	void run();
};
