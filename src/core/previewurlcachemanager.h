/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libruqolacore_export.h"
#include <QObject>
class RocketChatAccount;
class LIBRUQOLACORE_EXPORT PreviewUrlCacheManager : public QObject
{
    Q_OBJECT
public:
    explicit PreviewUrlCacheManager(RocketChatAccount *account, QObject *parent = nullptr);
    ~PreviewUrlCacheManager() override;

    [[nodiscard]] int embedCacheExpirationDays() const;
    void setEmbedCacheExpirationDays(int newEmbedCacheExpirationDays);

    [[nodiscard]] QString cachePath() const;
    void setCachePath(const QString &newCachePath);

private:
    void saveLastCheckedDateTime();
    [[nodiscard]] bool needToCheck() const;
    void checkCache();
    QString mCachePath;
    int mEmbedCacheExpirationDays = -1;
    RocketChatAccount *const mRocketChatAccount;
};
