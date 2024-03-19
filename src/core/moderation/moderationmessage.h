/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolacore_export.h"
#include <QDebug>
#include <QString>

class LIBRUQOLACORE_EXPORT ModerationMessage : public QSharedData
{
public:
    explicit ModerationMessage(const ModerationMessage &other);
    ModerationMessage();
    ~ModerationMessage();

    [[nodiscard]] QString moderationId() const;
    void setModerationId(const QString &newModerationId);

    void parse(const QJsonObject &obj);

    [[nodiscard]] QString roomId() const;
    void setRoomId(const QString &newRoomId);

    [[nodiscard]] QString roomName() const;
    void setRoomName(const QString &newRoomName);

    [[nodiscard]] QString roomFName() const;
    void setRoomFName(const QString &newRoomFName);

    [[nodiscard]] bool isEmpty() const;
    [[nodiscard]] bool operator==(const ModerationMessage &other) const;

private:
    LIBRUQOLACORE_NO_EXPORT void parseRoom(const QJsonObject &roomObj);
    QString mModerationId;
    QString mRoomId;
    QString mRoomName;
    QString mRoomFName;
};
LIBRUQOLACORE_EXPORT QDebug operator<<(QDebug d, const ModerationMessage &t);
