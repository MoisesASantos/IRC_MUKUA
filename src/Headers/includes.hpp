/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   includes.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosantos <mosantos@student.42luanda.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:38:24 by mosantos          #+#    #+#             */
/*   Updated: 2026/07/22 15:50:49 by mosantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

typedef struct sockaddr_in {
 
	sa_family_t     sin_family;
 	in_port_t       sin_port;
 	struct  in_addr sin_addr;
 	char            sin_zero[8];
};

typedef struct in_addr {
	
	in_addr_t s_addr;
};

typedef struct pollfd {
	
	int     fd;
	short   events;
	short   revents;
};
