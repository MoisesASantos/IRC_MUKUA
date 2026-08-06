#include "Channel.hpp"

Channel::Channel() : _name(""), _inviteOnly(false), _topicRestricted(false), _userLimit(0) {}

Channel::Channel(const std::string& name) : _name(name), _inviteOnly(false), _topicRestricted(false), _userLimit(0) {}

Channel::Channel(const Channel& other) { *this = other; }

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		_name = other._name;
		_topic = other._topic;
		_password = other._password;
		_members = other._members;
		_operators = other._operators;
		_invited = other._invited;
		_inviteOnly = other._inviteOnly;
		_topicRestricted = other._topicRestricted;
		_userLimit = other._userLimit;
	}
	return *this;
}

Channel::~Channel() {}

// Métodos Originais
std::string Channel::getName() const { return _name; }
void Channel::setTopic(const std::string& topic) { _topic = topic; }
std::string Channel::getTopic() const { return _topic; }

void Channel::addMember(Client* client) { _members.insert(client); }
void Channel::removeMember(Client* client) {
	_members.erase(client);
	_invited.erase(client);
	_operators.erase(client);
}
bool Channel::hasMember(Client* client) const { return _members.find(client) != _members.end(); }

void Channel::addOperator(Client* client) { _operators.insert(client); }
void Channel::removeOperator(Client* client) { _operators.erase(client); }
bool Channel::isOperator(Client* client) const { return _operators.find(client) != _operators.end(); }

void Channel::invite(Client* client) { _invited.insert(client); }
bool Channel::isInvited(Client* client) const { return _invited.find(client) != _invited.end(); }

void Channel::setInviteOnly(bool val) { _inviteOnly = val; }
bool Channel::isInviteOnly() const { return _inviteOnly; }
void Channel::setTopicRestricted(bool val) { _topicRestricted = val; }
bool Channel::isTopicRestricted() const { return _topicRestricted; }
void Channel::setPassword(const std::string& key) { _password = key; }
std::string Channel::getPassword() const { return _password; }
void Channel::setUserLimit(size_t limit) { _userLimit = limit; }
size_t Channel::getUserLimit() const { return _userLimit; }
size_t Channel::getMemberCount() const { return _members.size(); }

void Channel::broadcast(const std::string& msg, Client* exclude) {
	for (std::set<Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
		if (*it != exclude)
			(*it)->sendData(msg + "\r\n");
	}
}
