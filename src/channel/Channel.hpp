#pragma once

#include <string>
#include <set>
#include "../Client/Client.hpp"

class Channel {
private:
	std::string _name;
	std::string _topic;
	std::string _password;
	std::set<Client*> _members;
	std::set<Client*> _operators;
	std::set<Client*> _invited;

	bool _inviteOnly;
	bool _topicRestricted;
	size_t _userLimit;

public:
	Channel();
	Channel(const std::string& name);
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
	~Channel();

	std::string getName() const;
	void setTopic(const std::string& topic);
	std::string getTopic() const;

	void addMember(Client* client);
	void removeMember(Client* client);
	bool hasMember(Client* client) const;

	void addOperator(Client* client);
	void removeOperator(Client* client);
	bool isOperator(Client* client) const;

	void invite(Client* client);
	bool isInvited(Client* client) const;

	void setInviteOnly(bool val);
	bool isInviteOnly() const;
	void setTopicRestricted(bool val);
	bool isTopicRestricted() const;
	void setPassword(const std::string& key);
	std::string getPassword() const;
	void setUserLimit(size_t limit);
	size_t getUserLimit() const;
	size_t getMemberCount() const;
	std::string getNamesList();

	void broadcast(const std::string& msg, Client* exclude = NULL);
};

