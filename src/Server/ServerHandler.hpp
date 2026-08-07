/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgaspar <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 15:18:08 by sgaspar           #+#    #+#             */
/*   Updated: 2026/08/07 19:04:03 by sgaspar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include "../Client/Client.hpp"
#include "./Server.hpp"

class	ServerHandler {
	public:
	enum NumericReplies {
		// --- INFORMATIVOS (RPL) ---
		RPL_WELCOME           = 1,
		RPL_YOURHOST          = 2,
		RPL_CREATED           = 3,
		RPL_MYINFO            = 4,

		RPL_CHANNELMODEIS     = 324,
		RPL_NOTOPIC           = 331,
		RPL_TOPIC             = 332,
		RPL_INVITING          = 341,
		RPL_NAMREPLY          = 353,
		RPL_ENDOFNAMES        = 366,

		RPL_MOTD              = 372,
		RPL_MOTDSTART         = 375,
		RPL_ENDOFMOTD         = 376,

		// --- ERROS (ERR) ---
		ERR_NOSUCHNICK        = 401,
		ERR_NOSUCHCHANNEL     = 403,
		ERR_CANNOTSENDTOCHAN  = 404,
		ERR_NOORIGIN          = 409,
		ERR_NORECIPIENT       = 411,
		ERR_NOTEXTTOSEND      = 412,
		ERR_UNKNOWNCOMMAND    = 421,
		ERR_NOMOTD            = 422,
		ERR_NONICKNAMEGIVEN   = 431,
		ERR_ERRONEUSNICK      = 432,
		ERR_NICKNAMEINUSE     = 433,
		ERR_USERNOTINCHANNEL  = 441,
		ERR_NOTONCHANNEL      = 442,
		ERR_USERONCHANNEL     = 443,
		ERR_NOTREGISTERED     = 451,
		ERR_NEEDMOREPARAMS    = 461,
		ERR_ALREADYREGISTRED  = 462,
		ERR_PASSWDMISMATCH    = 464,
		ERR_KEYSET            = 467,
		ERR_CHANNELISFULL     = 471,
		ERR_UNKNOWNMODE       = 472,
		ERR_INVITEONLYCHAN    = 473,
		ERR_BADCHANNELKEY     = 475,
		ERR_CHANOPRIVSNEEDED  = 482
	};

	private:
	Server	*server;
	std::map<std::string, Channel> *channels;
	void tryRegister(Client* client);
	void sendReply(Client* client, NumericReplies code, const std::string& arg1 = "", const std::string& arg2 = "");


	public:
	ServerHandler();
	ServerHandler(const ServerHandler& other);
	ServerHandler& operator=(const ServerHandler & other);
	~ServerHandler();

	void	setServer(Server *server);

	void processCommand(Client* client, const std::string& msg);
	std::vector<std::string> splitMessage(const std::string& msg);

	void cmdPass(Client* client, const std::vector<std::string>& args);
	void cmdNick(Client* client, const std::vector<std::string>& args);
	void cmdUser(Client* client, const std::vector<std::string>& args);
	void cmdJoin(Client* client, const std::vector<std::string>& args);
	void cmdPrivmsg(Client* client, const std::vector<std::string>& args);
	void cmdKick(Client* client, const std::vector<std::string>& args);
	void cmdInvite(Client* client, const std::vector<std::string>& args);
	void cmdTopic(Client* client, const std::vector<std::string>& args);
	void cmdMode(Client* client, const std::vector<std::string>& args);


};
