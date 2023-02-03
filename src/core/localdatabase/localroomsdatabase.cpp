/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "localroomsdatabase.h"
#include "config-ruqola.h"
#include "localdatabaseutils.h"
#include "ruqola_debug.h"

static const char s_schemaRoomDataBase[] = "CREATE TABLE ROOMS (roomId TEXT PRIMARY KEY NOT NULL, timestamp INTEGER, json TEXT)";
enum class Fields {
    RoomId,
    TimeStamp,
    Json,
}; // in the same order as the table

LocalRoomsDatabase::LocalRoomsDatabase()
    : LocalDatabaseBase(LocalDatabaseUtils::localRoomsDatabasePath())
{
}

LocalRoomsDatabase::~LocalRoomsDatabase() = default;

void LocalRoomsDatabase::addRoom(const QString &roomId)
{
}

void LocalRoomsDatabase::deleteRoom(const QString &roomId)
{
}

QString LocalRoomsDatabase::databaseName(const QString &name)
{
    return QStringLiteral("rooms-") + name;
}
