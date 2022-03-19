/*
   SPDX-FileCopyrightText: 2017-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqola_private_export.h"
#include <QDebug>
class LIBRUQOLACORE_TESTS_EXPORT MessageUrl
{
    Q_GADGET
public:
    MessageUrl();
    Q_REQUIRED_RESULT bool operator==(const MessageUrl &other) const;
    static QJsonObject serialize(const MessageUrl &url);
    static MessageUrl fromJSon(const QJsonObject &o);

    Q_REQUIRED_RESULT QString url() const;
    void setUrl(const QString &url);

    Q_REQUIRED_RESULT QString pageTitle() const;
    void setPageTitle(const QString &pageTitle);

    Q_REQUIRED_RESULT bool isEmpty() const;

    Q_REQUIRED_RESULT QString description() const;
    void setDescription(const QString &description);

private:
    QString mUrl;
    QString mPageTitle;
    QString mDescription;
};
LIBRUQOLACORE_EXPORT QDebug operator<<(QDebug d, const MessageUrl &t);
Q_DECLARE_TYPEINFO(MessageUrl, Q_MOVABLE_TYPE);
