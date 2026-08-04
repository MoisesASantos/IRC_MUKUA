/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emjoao <emjoao@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 10:10:39 by emjoao            #+#    #+#             */
/*   Updated: 2026/08/04 10:23:35 by emjoao           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
    : _inviteOnly(false), _topicLocked(false), _hasKey(false), _userLimit(0) {}

Channel::Channel(const std::string& name)
    : _name(name), _inviteOnly(false), _topicLocked(false), _hasKey(false), _userLimit(0) {}

Channel::~Channel() {}

const std::string& Channel::GetName() const { return _name; }
const std::string& Channel::GetTopic() const { return _topic; }
bool Channel::IsInviteOnly() const { return _inviteOnly; }
bool Channel::IsTopicLocked() const { return _topicLocked; }
bool Channel::HasKey() const { return _hasKey; }
const std::string& Channel::GetKey() const { return _key; }
int Channel::GetUserLimit() const { return _userLimit; }
size_t Channel::GetMemberCount() const { return _members.size(); }

void Channel::SetTopic(const std::string& topic) { _topic = topic; }
void Channel::SetInviteOnly(bool value) { _inviteOnly = value; }
void Channel::SetTopicLocked(bool value) { _topicLocked = value; }
void Channel::SetKey(const std::string& key) { _key = key; _hasKey = true; }
void Channel::RemoveKey() { _key.clear(); _hasKey = false; }
void Channel::SetUserLimit(int limit) { _userLimit = limit; }

void Channel::AddMember(Client* client)
{
    _members[client->GetFd()] = client;
}

void Channel::RemoveMember(int fd)
{
    _members.erase(fd);
    _operators.erase(fd);
}

bool Channel::IsMember(int fd) const
{
    return _members.find(fd) != _members.end();
}

void Channel::AddOperator(int fd)
{
    if (IsMember(fd))
        _operators[fd] = _members[fd];
}

void Channel::RemoveOperator(int fd)
{
    _operators.erase(fd);
}

bool Channel::IsOperator(int fd) const
{
    return _operators.find(fd) != _operators.end();
}

void Channel::Invite(int fd)
{
    _invited.push_back(fd);
}

bool Channel::IsInvited(int fd) const
{
    for (size_t i = 0; i < _invited.size(); i++)
        if (_invited[i] == fd)
            return true;
    return false;
}

void Channel::Broadcast(const std::string& message, int excludeFd)
{
    std::map<int, Client*>::iterator it;
    for (it = _members.begin(); it != _members.end(); ++it)
    {
        if (it->first != excludeFd)
            it->second->SendReply(message);
    }
}