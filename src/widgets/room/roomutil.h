/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolawidgets_private_export.h"
#include "room.h"
#include <QString>
namespace RoomUtil
{
Q_REQUIRED_RESULT LIBRUQOLAWIDGETS_TESTS_EXPORT QString generateUserLink(const QString &userName);
Q_REQUIRED_RESULT LIBRUQOLAWIDGETS_TESTS_EXPORT QString generatePermalink(const QString &messageId, const QString &roomId, Room::RoomType channelType);
}
