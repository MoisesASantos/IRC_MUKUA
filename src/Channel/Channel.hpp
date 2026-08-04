/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emjoao <emjoao@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 09:58:43 by emjoao            #+#    #+#             */
/*   Updated: 2026/08/04 10:03:30 by emjoao           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../Headers/includes.hpp"
#include "../Client/Client.hpp"

class Channel
{
private:
    std::string             _name;
    std::string             _topic;
    std::string             _key;              // password do canal (modo +k)
    std::map<int, Client*>  _members;           // fd -> Client*
    std::map<int, Client*>  _operators;         // fd -> Client* (subset de _members)
    std::vector<int>        _invited;           // fds convidados (para modo +i)

    bool _inviteOnly;   // +i
    bool _topicLocked;  // +t 
    bool _hasKey;       // +k
    int  _userLimit;    // +l 

public:
    Channel();
    Channel(const std::string& name);
    ~Channel();

    const std::string& GetName() const;
    const std::string& GetTopic() const;
    bool IsInviteOnly() const;
    bool IsTopicLocked() const;
    bool HasKey() const;
    const std::string& GetKey() const;
    int GetUserLimit() const;
    size_t GetMemberCount() const;

    void SetTopic(const std::string& topic);
    void SetInviteOnly(bool value);
    void SetTopicLocked(bool value);
    void SetKey(const std::string& key);
    void RemoveKey();
    void SetUserLimit(int limit);

    void AddMember(Client* client);
    void RemoveMember(int fd);
    bool IsMember(int fd) const;

    void AddOperator(int fd);
    void RemoveOperator(int fd);
    bool IsOperator(int fd) const;

    void Invite(int fd);
    bool IsInvited(int fd) const;

    void Broadcast(const std::string& message, int excludeFd = -1);
};