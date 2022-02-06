/*
   SPDX-FileCopyrightText: 2021-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "createroominfo.h"

#include <QJsonArray>
#include <QJsonObject>

using namespace RocketChatRestApi;

bool CreateRoomInfo::isValid() const
{
    return !name.isEmpty();
}

bool CreateRoomInfo::canStart() const
{
    return isValid();
}

QJsonDocument CreateRoomInfo::json(bool createTeam) const
{
    QJsonObject jsonObj;
    if (!members.isEmpty()) {
        jsonObj[QLatin1String("members")] = QJsonArray::fromStringList(members);
    }
    jsonObj[QLatin1String("name")] = name;
    if (readOnly) {
        jsonObj[QLatin1String("readOnly")] = true;
    } // Default is false

    if (createTeam) {
        jsonObj[QLatin1String("type")] = privateChannel ? 1 : 0;
    }

    QJsonObject extraJsonObj;

    if (broadcast) {
        extraJsonObj[QLatin1String("broadcast")] = true;
    } // Default is false
    if (encrypted) {
        extraJsonObj[QLatin1String("encrypted")] = true;
    } // Default is false
    if (!description.isEmpty()) {
        extraJsonObj[QLatin1String("description")] = description;
    }
    if (!teamId.isEmpty()) {
        extraJsonObj[QLatin1String("teamId")] = teamId;
    }
    jsonObj[QLatin1String("extraData")] = extraJsonObj;
    const QJsonDocument postData = QJsonDocument(jsonObj);
    return postData;
}

QDebug operator<<(QDebug d, const RocketChatRestApi::CreateRoomInfo &t)
{
    d << "name " << t.name;
    d << "description " << t.description;
    d << "teamId " << t.teamId;
    d << "members " << t.members;
    d << "readOnly " << t.readOnly;
    d << "broadcast " << t.broadcast;
    d << "encrypted " << t.encrypted;
    d << "private channel " << t.privateChannel;
    return d;
}
