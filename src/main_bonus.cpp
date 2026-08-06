/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgaspar <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 15:17:35 by sgaspar           #+#    #+#             */
/*   Updated: 2026/08/06 15:17:46 by sgaspar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Bot/Bot.hpp"
#include <iostream>
#include <cstdlib>

int	main(int argc, char **argv) {
	if (argc != 4) {
		std::cerr << "Usage: ./ircbot <ip> <port> <password>" << std::endl;
		return 1;
	}
	try {
		Bot bot(argv[1], std::atoi(argv[2]), argv[3]);
		bot.run();
	} catch (std::exception &e) {
		std::cerr << "Bot Error: " << e.what() << std::endl;
	}
	return 0;
}
