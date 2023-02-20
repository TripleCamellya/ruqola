/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "localdatabaseutils.h"

#include <QStandardPaths>

QString LocalDatabaseUtils::fixRoomName(QString roomName)
{
    roomName.remove(QLatin1Char('/'));
    roomName.remove(QLatin1Char(':'));
    return roomName;
}

QString LocalDatabaseUtils::localMessageLoggerPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QStringLiteral("/logs/");
}

QString LocalDatabaseUtils::localDatabasePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QStringLiteral("/database/");
}

QString LocalDatabaseUtils::localMessagesDatabasePath()
{
    return LocalDatabaseUtils::localDatabasePath() + LocalDatabaseUtils::databasePath(LocalDatabaseUtils::DatabasePath::Messages);
}

QString LocalDatabaseUtils::localRoomsDatabasePath()
{
    return LocalDatabaseUtils::localDatabasePath() + LocalDatabaseUtils::databasePath(LocalDatabaseUtils::DatabasePath::Rooms);
}

QString LocalDatabaseUtils::localAccountDatabasePath()
{
    return LocalDatabaseUtils::localDatabasePath() + LocalDatabaseUtils::databasePath(LocalDatabaseUtils::DatabasePath::Account);
}

QString LocalDatabaseUtils::localGlobalDatabasePath()
{
    return LocalDatabaseUtils::localDatabasePath() + LocalDatabaseUtils::databasePath(LocalDatabaseUtils::DatabasePath::Global);
}

QString LocalDatabaseUtils::databasePath(LocalDatabaseUtils::DatabasePath pathType)
{
    switch (pathType) {
    case LocalDatabaseUtils::DatabasePath::Messages:
        return QStringLiteral("messages/");
    case LocalDatabaseUtils::DatabasePath::Rooms:
        return QStringLiteral("rooms/");
    case LocalDatabaseUtils::DatabasePath::Account:
        return QStringLiteral("account/");
    case LocalDatabaseUtils::DatabasePath::Global:
        return QStringLiteral("global/");
    }
    Q_ASSERT(true);
    return {};
}
