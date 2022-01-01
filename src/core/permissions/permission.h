/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqola_private_export.h"
#include "roles/roleinfo.h"
#include <QDebug>
#include <QJsonObject>
#include <QString>

class LIBRUQOLACORE_TESTS_EXPORT Permission
{
public:
    Permission();

    bool parsePermission(const QJsonObject &replyObject, const QVector<RoleInfo> &roleInfo = {}, bool restApi = true);
    Q_REQUIRED_RESULT QStringList roles() const;
    void setRoles(const QStringList &newRoles);

    Q_REQUIRED_RESULT qint64 updatedAt() const;
    void setUpdatedAt(qint64 newUpdatedAt);

    Q_REQUIRED_RESULT bool isValid() const;

    Q_REQUIRED_RESULT const QString &identifier() const;
    void setIdentifier(const QString &newIdentifier);

    Q_REQUIRED_RESULT const QStringList &rolesStr() const;

    Q_REQUIRED_RESULT bool operator==(const Permission &other) const;

private:
    qint64 mUpdatedAt = -1;
    QStringList mRolesStr;
    QStringList mRoles;
    QString mIdentifier;
};
Q_DECLARE_METATYPE(Permission)
Q_DECLARE_TYPEINFO(Permission, Q_MOVABLE_TYPE);
LIBRUQOLACORE_EXPORT QDebug operator<<(QDebug d, const Permission &t);
