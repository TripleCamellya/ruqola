/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "moderationmessagesmodel.h"
#include "moderationlistmessages.h"

ModerationMessagesModel::ModerationMessagesModel(RocketChatAccount *account, QObject *parent)
    : CommonMessagesModel{account, parent}
{
}

ModerationMessagesModel::~ModerationMessagesModel() = default;

QVector<Message> ModerationMessagesModel::extractMessages(const QJsonObject &obj)
{
    ModerationListMessages messages;
    messages.parseMessages(obj, QStringLiteral("messages"));
    return messages.listMessages();
}
