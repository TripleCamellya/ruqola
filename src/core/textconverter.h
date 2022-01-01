/*
   SPDX-FileCopyrightText: 2018-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolacore_export.h"
#include <QString>
#include <QVector>

class EmojiManager;
class Message;
class MessageCache;

namespace TextConverter
{
LIBRUQOLACORE_EXPORT Q_REQUIRED_RESULT QString convertMessageText(const QString &str,
                                                                  const QString &userName,
                                                                  const QVector<Message> &allMessages,
                                                                  const QStringList &highlightWords,
                                                                  EmojiManager *emojiManager,
                                                                  MessageCache *messageCache,
                                                                  QString &needUpdateMessageId);
}

